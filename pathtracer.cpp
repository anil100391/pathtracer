#include "ray.h"
#include "scene.h"
#include "sphere.h"
#include "vec3.h"
#include <iostream>
#include <cassert>
#include <random>
#include <chrono>

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

    // DIFFUSE COMPONENT
    vec3f w = h._normal;
    vec3f u;

    if ( w[0] > w[1] && w[0] > w[2] )
        u = w * vec3f(0, 1, 0);
    else if ( w[1] > w[0] && w[1] > w[2] )
        u = w * vec3f(0, 0, 1);
    else
        u = w * vec3f(1, 0, 0);

    vec3f v = w * u;

    auto range = (rng.max() - rng.min());
    float r1 = 2.0f * M_PI * (1.0f * (rng() - rng.min())  / range);
    float r2 = 1.0f * (rng() - rng.min()) / range;
    float r2s = std::sqrt(r2);
    newray.d = u*std::cos(r1)*r2s + v*std::sin(r1)*r2s + w*std::sqrt(1 - r2);
    newray.d.normalize();

    color emit = h._mat.diffuse() * h._mat.emission();
    color diffuse = h._mat.diffuse();
    color incoming = tracepath(world, rng, newray, depth + 1);

    // SPECULAR COMPONENT
    newray.d = r.d - h._normal * 2 * (r.d % h._normal);
    newray.d.normalize();
    incoming =  incoming + tracepath(world, rng, newray, depth + 1);
    return emit + diffuse * incoming;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main()
{
    unsigned int width  = 256;
    unsigned int height = 256;

    vec3f  cameraPos = {3.672f, 0.0f, 0.0f};

    scenef world;
    material whiteDiffuse({1.0f, 1.0f, 1.0f, 1.0f});
    material redDiffuse({1.0f, 0.0f, 0.0f, 1.0f});
    material whiteEmissive({1.0f, 1.0f, 1.0f, 1.0f});
    whiteEmissive.setEmissive(50.0f);

    world << new sphere<FLOAT>(
                 {0.4f, 0.0f, -0.6f}, 0.75f * 0.5f, whiteDiffuse )
          << new sphere<FLOAT>(
                 {-0.4f, 0.5f, -0.6f}, 0.75f * 0.5f, redDiffuse )
          << new sphere<FLOAT>(
                 {-0.4f, -0.5f, -0.6f}, 0.75f * 0.5f, redDiffuse )
          << new sphere<FLOAT>(
                 {0.0f, 0.0f, 1.0f}, 0.75f * 0.5f, whiteEmissive );

    auto begin = std::chrono::high_resolution_clock::now();
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
            float py  = 1.0f * ii / ( width - 1 ) - 0.5f;
            float pz  = 1.0f * (width -1 - jj) / ( width - 1 ) - 0.5f;
            float px  = -1.0f;
            vec3f dir = {px, py, pz};
            dir.normalize();
            rayf r = {cameraPos, dir};
            color pixcolor = {0, 0, 0, 0};
            auto now = std::chrono::high_resolution_clock::now();
            std::mt19937 mtrng((now - begin).count());
            unsigned int numSamples = 128;
            for ( unsigned int sample = 0; sample < numSamples; ++sample )
            {
                pixcolor = pixcolor + tracepath(world, mtrng, r, 0);
            }
            pixcolor = pixcolor / ( numSamples * MAX_DEPTH );
            vec3<unsigned char> c = pixcolor.touchar();
            fprintf( image, "%c%c%c", c[0], c[1], c[2] );
        }
    }

    fclose( image );
    return 0;
}
