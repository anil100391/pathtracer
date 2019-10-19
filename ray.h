#ifndef _ray_h_
#define _ray_h_

#include "vec3.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class ray
{
public:

    constexpr ray() noexcept = default;
    constexpr ray( const vec3<T> &origin, const vec3<T> &dir ) noexcept
        : o( origin ), d( dir )
    {
    }

    vec3<T> o;                      // origin
    vec3<T> d = {T(1), T(0), T(0)}; // direction
};

#endif // _ray_h_
