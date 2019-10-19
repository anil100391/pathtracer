#include "ray.h"
#include "vec3.h"
#include "sphere.h"
#include "scene.h"
#include <iostream>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using vec3f = vec3<float>;
using rayf = ray<float>;
using scenef = scene<float>;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main()
{
    unsigned int width  = 512;
    unsigned int height = 512;

    vec3f cameraPos = {0.0f, 0.0f, 0.0f};
    scenef world;
    world << new sphere<float>({0.0f, 0.0f, 4.0f}, 0.5f) << new sphere<float>({1.0f, 1.0f, 4.0f}, 0.25f);

    hit<float> h;
    for ( unsigned int jj = 0; jj < height; ++jj )
    {
        for ( unsigned int ii = 0; ii < width; ++ii )
        {
            // build ray for pixel (ii, jj)
            double px = 1.0 * ii / (width - 1) - 0.5;
            double py = 1.0 * jj / (width - 1) - 0.5;
            double pz = 1.0;
            vec3f dir = {px, py, pz};
            dir.normalize();
            rayf r = {cameraPos, dir};
            if ( !world.intersect(r, h) )
            {
                std::cout << ii << ", " << jj << "\n";
            }
        }
    }
}
