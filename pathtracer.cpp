#include "ray.h"
#include "scene.h"
#include "sphere.h"
#include "vec3.h"
#include <iostream>
#include <cassert>
#include <random>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using FLOAT = double;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
constexpr unsigned int MAX_DEPTH = 16;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using vec3f  = vec3<FLOAT>;
using rayf   = ray<FLOAT>;
using hitf   = hit<FLOAT>;
using scenef = scene<FLOAT>;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename randomNumGen>
static color tracepath(scenef& world, randomNumGen& rng, const rayf &r, unsigned int depth)
{
    if ( depth >= MAX_DEPTH )
        return {0, 0, 0, 0};

    hitf h;
    if ( !world.intersect(r, h) )
        return {0, 0, 0, 0};

    rayf newray{h._pos, h._normal};
    auto range = (rng.max() - rng.min());
    float x = 1.0f * rng() / range - 0.5f;
    float y = 1.0f * rng() / range - 0.5f;
    float z = 1.0f * rng() / range - 0.5f;
    vec3f rd = {x, y, z};
    rd.normalize();
    if ( rd % h._normal < 0 )
        newray.d = rd * -1.0f;
    else
        newray.d = rd;

    // From wikipedia
    // Probability of the newray
    const float p = 1/(2*M_PI);

    // Compute the BRDF for this ray (assuming Lambertian reflection)
    float cos_theta = newray.d % h._normal;
    // color BRDF = material.reflectance / M_PI ;
    color matreflectance = {1.0f, 1.0f, 1.0f, 1.0f};
    color BRDF = matreflectance / M_PI ;

    // Recursively trace reflected light sources.
    color incoming = tracepath(world, rng, newray, depth + 1);

    color matemittance   = h._mat.diffuse() * (1 + h._mat.emission());
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
    material purpleDiffuse({1.0f, 0.0f, 1.0f, 1.0f});
    material greenEmissive({1.0f, 1.0f, 1.0f, 1.0f});
    greenEmissive.setEmissive(50.0f);

    world << new sphere<FLOAT>( {0.5f, 0.0f, 4.5f}, 0.5f, purpleDiffuse )
          << new sphere<FLOAT>( {-0.5f, 0.0f, 3.0f}, 0.5f, greenEmissive );

    FILE *image = fopen( "render.ppm", "w" );
    fprintf( image, "P6\n%d %d\n255\n", width, height );
    float progPercent = 0.0f;
    for ( unsigned int jj = 0; jj < height; ++jj )
    {
        float curProg = (100.0f * jj / (height - 1));
        if ( curProg > progPercent )
        {
            progPercent += 10;
            std::cout << "[" << progPercent  << "% ...]" << "\n";
        }

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
            std::mt19937 mtrng;
            unsigned int numSamples = 512;
            for ( unsigned int sample = 0; sample < numSamples; ++sample )
            {
                pixcolor = pixcolor + tracepath(world, mtrng, r, 0);
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
