#include "camera.h"
#include "mesh.h"
#include "ray.h"
#include "scene.h"
#include "sphere.h"
#include "vec3.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <random>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using FLOAT = double;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
constexpr unsigned int MAX_DEPTH = 4;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using vec3f  = vec3<FLOAT>;
using rayf   = ray<FLOAT>;
using hitf   = hit<FLOAT>;
using scenef = scene<FLOAT>;
using meshf  = mesh<FLOAT>;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
static void buildCornellBoxScene( scenef &world )
{
    material redDiffuse{{1.0f, 0.0f, 0.0f, 1.0f}};
    material blueDiffuse{{1.0f, 0.0f, 1.0f, 1.0f}};
    material greenDiffuse{{1.0f, 1.0f, 0.0f, 1.0f}};
    material whiteEmissive{{1.0f, 1.0f, 1.0f, 1.0f}};
    whiteEmissive.setEmissive( 50.0f );

    auto cornellbox = new meshf( "/home/nebula/code/path_tracer/cornellbox.obj" );
    cornellbox->transform(mat44<FLOAT>::makeRotation(90, 2));
    world
        << new sphere<FLOAT>( {0.4f, 0.0f, -0.6f}, 0.75f * 0.5f, redDiffuse )
        << new sphere<FLOAT>( {-0.4f, 0.5f, -0.6f}, 0.75f * 0.5f, blueDiffuse )
        << new sphere<FLOAT>(
               {-0.4f, -0.5f, -0.6f}, 0.75f * 0.5f, greenDiffuse )
        << new sphere<FLOAT>( {0.0f, 0.0f, 1.25f}, 0.75f * 0.5f, whiteEmissive )
        << cornellbox;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename randomNumGen>
static color
tracepath( scenef &world, randomNumGen &rng, const rayf &r, unsigned int depth )
{
    if ( depth >= MAX_DEPTH )
        return {0, 0, 0, 0};

    hitf h;
    if ( !world.intersect( r, h ) )
        return {0, 0, 0, 0};

    rayf newray{h._pos, h._normal};

    // DIFFUSE COMPONENT
    vec3f w = h._normal;
    vec3f u;

    auto x = std::abs(w[0]);
    auto y = std::abs(w[1]);
    auto z = std::abs(w[2]);

    if ( x > y && x > z )
        u = w * vec3f( 0, 1, 0 );
    else if ( y > x && y > z )
        u = w * vec3f( 0, 0, 1 );
    else
        u = w * vec3f( 1, 0, 0 );

    vec3f v = w * u;
    assert ( v.len2() != 0 );

    auto  range = ( rng.max() - rng.min() );
    float r1    = 2.0f * M_PI * ( 1.0f * ( rng() - rng.min() ) / range );
    float r2    = 1.0f * ( rng() - rng.min() ) / range;
    float r2s   = std::sqrt( r2 );
    newray.d    = u * std::cos( r1 ) * r2s + v * std::sin( r1 ) * r2s +
               w * std::sqrt( 1 - r2 );
    newray.d.normalize();

    color emit     = h._mat.diffuse() * h._mat.emission();
    color diffuse  = h._mat.diffuse();
    color incoming = tracepath( world, rng, newray, depth + 1 );

    // SPECULAR COMPONENT
    // newray.d = r.d - h._normal * 2 * (r.d % h._normal);
    // newray.d.normalize();
    // incoming =  incoming + tracepath(world, rng, newray, depth + 1);
    return emit + diffuse * incoming;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main()
{
    unsigned int width  = 512;
    unsigned int height = 512;

    float coord = 2.472f;
    camera<FLOAT> cam{{coord, 0, 0}, {0, 0, 0}, 60};

    scenef world;
    buildCornellBoxScene( world );

    std::unique_ptr<FILE, decltype( &fclose )> image(
        fopen( "render.ppm", "w" ), &fclose );

    fprintf( image.get(), "P6\n%d %d\n255\n", width, height );
    float progPercent = 0.0f;
    for ( unsigned int jj = 0; jj < height; ++jj )
    {
        float curProg = ( 100.0f * jj / ( height - 1 ) );
        if ( curProg > progPercent )
        {
            progPercent += 10;
            std::cout << "[" << progPercent << "% ...]"
                      << "\n";
        }

        for ( unsigned int ii = 0; ii < width; ++ii )
        {
            color        pixcolor   = {0, 0, 0, 0};
            unsigned int numSamples = 512;
            // subpixel y
            for ( unsigned int spy = 0; spy < 2; ++spy )
            {
                // subpixel x
                for ( unsigned int spx = 0; spx < 2; ++spx )
                {
                    std::mt19937 mtrng( jj * width + ii );
                    float        dx =
                        1.0f * mtrng() / ( mtrng.max() - mtrng.min() ) - 0.5f;
                    float dy =
                        1.0f * mtrng() / ( mtrng.max() - mtrng.min() ) - 0.5f;
                    // build ray for pixel (ii, jj)
                    FLOAT u = 1.0f * ( ii + dx ) / ( width - 1 ) - 0.5f;
                    FLOAT v =
                        1.0f * ( width - 1 - ( jj + dy ) ) / ( width - 1 ) -
                        0.5f;
                    vec3f dir = cam.direction( u, v );
                    dir.normalize();
                    rayf r = {cam.position(), dir};
                    for ( unsigned int sample = 0; sample < numSamples;
                          ++sample )
                    {
                        pixcolor = pixcolor + tracepath( world, mtrng, r, 0 );
                    }
                }
            }

            pixcolor              = pixcolor / ( 4 * numSamples * MAX_DEPTH );
            vec3<unsigned char> c = pixcolor.touchar();
            fprintf( image.get(), "%c%c%c", c[0], c[1], c[2] );
        }
    }

    return 0;
}
