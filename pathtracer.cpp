#include "ray.h"
#include "scene.h"
#include "sphere.h"
#include "vec3.h"
#include <iostream>
#include <cassert>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
constexpr unsigned int MAX_DEPTH = 4;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using vec3f  = vec3<float>;
using rayf   = ray<float>;
using hitf   = hit<float>;
using scenef = scene<float>;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct color
{
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 1.0f;

    color operator/(float scale)
    {
        return {r/scale, g/scale, b/scale, a/scale};
    }

    color operator*(float scale)
    {
        return {r*scale, g*scale, b*scale, a*scale};
    }

    color operator+(const color& other)
    {
        return {r + other.r, g+other.g, b+other.b, a+other.a};
    }

    color operator*(const color& other)
    {
        return {r*other.r, g*other.g, b*other.b, a*other.a};
    }

    void clip()
    {
        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (g < 0) b = 0;
        if (a < 0) a = 0;

        if (r > 1.0f) r = 1.0f;
        if (g > 1.0f) g = 1.0f;
        if (g > 1.0f) b = 1.0f;
        if (a > 1.0f) a = 1.0f;
    }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static color tracepath(scenef& world, const rayf &r, unsigned int depth)
{
    if ( depth >= MAX_DEPTH )
        return {0, 0, 0, 0};

    hit<float> h;
    if ( !world.intersect(r, h) )
        return {0, 0, 0, 0};

    rayf newray{h._pos, h._normal};
    float x = rand();
    float y = rand();
    float z = rand();
    vec3f rd = {x, y, z};
    rd.normalize();
    if ( rd % h._normal < 0 )
        newray.d = rd * -1.0f;
    else
        newray.d = rd;

    // Probability of the newray
    const float p = 1/(2*M_PI);

    // Compute the BRDF for this ray (assuming Lambertian reflection)
    float cos_theta = newray.d % h._normal;
    // color BRDF = material.reflectance / M_PI ;
    color matreflectance = {1.0f, 1.0f, 1.0f, 1.0f};
    // color matemittance   = {1.0f, 0, 0, 1.0f};
    color matemittance   = {h._r,  h._g, h._b, 1.0f};
    color BRDF = matreflectance / M_PI ;

    // Recursively trace reflected light sources.
    color incoming = tracepath(world, newray, depth + 1);

    // Apply the Rendering Equation here.
    return matemittance + (BRDF * incoming * cos_theta / p);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main()
{
    unsigned int width  = 512;
    unsigned int height = 512;

    vec3f  cameraPos = {0.0f, 0.0f, 0.0f};
    scenef world;
    world << new sphere<float>( {0.0f, 0.0f, 3.0f}, 0.5f )
          << new sphere<float>( {1.0f, 1.0f, 4.0f}, 0.25f );

    FILE *image = fopen( "render.ppm", "w" );
    fprintf( image, "P6\n%d %d\n255\n", width, height );
    for ( unsigned int jj = 0; jj < height; ++jj )
    {
        std::cout << "image row:" << jj << "\n";
        for ( unsigned int ii = 0; ii < width; ++ii )
        {
            // build ray for pixel (ii, jj)
            float px  = 1.0f * ii / ( width - 1 ) - 0.5f;
            float py  = 1.0f * jj / ( width - 1 ) - 0.5f;
            float pz  = 1.0f;
            vec3f dir = {px, py, pz};
            dir.normalize();
            rayf r = {cameraPos, dir};
            color pixcolor = {0, 0, 0, 0};
            unsigned int numSamples = 64;
            for ( unsigned int sample = 0; sample < numSamples; ++sample )
            {
                pixcolor = pixcolor + tracepath(world, r, 0);
            }
            pixcolor = pixcolor / ( numSamples * MAX_DEPTH );
            pixcolor.clip();
            unsigned char red = 255 * pixcolor.r;
            unsigned char green = 255 * pixcolor.g;
            unsigned char blue = 255 * pixcolor.b;

            fprintf( image, "%c%c%c", red, green, blue );
        }
    }

    fclose( image );
    return 0;
}
