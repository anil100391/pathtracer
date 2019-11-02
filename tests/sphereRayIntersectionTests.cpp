#include "../ray.h"
#include "../sphere.h"
#include <cassert>
#include <random>

using vec3f   = vec3<float>;
using rayf    = ray<float>;
using spheref = sphere<float>;

static std::mt19937 rng;
static auto         range = rng.max() - rng.min();

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
float randomlength() { return ( 1.0f * rng() / range ); }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
vec3f randomdirection()
{
    float x = randomlength() - 0.5f;
    float y = randomlength() - 0.5f;
    float z = randomlength() - 0.5f;
    vec3f v{x, y, z};
    v.normalize();
    return v;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main()
{
    vec3f   center{2.0f, 1.0f, 4.0f};
    spheref s( center, 2.0f );

    constexpr int numTrials = 100;

    for ( int ii = 0; ii < numTrials; ++ii )
    {
        vec3f dir = randomdirection();

        {
            // ray origin outside sphere
            vec3f randompointoutsideSphere =
                center + dir * ( 1.0f + s.radius() ) * randomlength();

            rayf       r( randompointoutsideSphere, dir * -1.0f );
            hit<float> h;
            bool       isec = s.intersect( r, h );
            assert( isec );
        }

        {
            // ray origin inside sphere
            vec3f randompointinsideSphere =
                center + dir * ( s.radius() ) * randomlength();

            rayf       r( randompointinsideSphere, dir * 1.0f );
            hit<float> h;
            bool       isec = s.intersect( r, h );
            assert( isec );
        }
    }

    std::cout << "All tests passed\n";
    return 0;
}
