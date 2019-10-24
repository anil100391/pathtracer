#ifndef _sphere_h_
#define _sphere_h_

#include "hit.h"
#include "material.h"
#include "primitive.h"
#include "ray.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class sphere : public primitive<T>
{
public:
    constexpr sphere() noexcept = default;
    constexpr sphere( const vec3<T> & center,
                      T               radius,
                      const material &mat = {} ) noexcept
        : _center( center ), _radius( radius ), _mat( mat )
    {
    }

    constexpr T radius() const noexcept { return _radius; }

    constexpr const vec3<T> &center() const noexcept { return _center; }

    constexpr virtual bool intersect( const ray<T> &r, hit<T> &h ) const
        noexcept override;

    constexpr const material &getMaterial() const noexcept { return _mat; }

private:
    vec3<T> _center = {T( 0 ), T( 0 ), T( 0 )};
    T       _radius = T( 1 );

    material _mat;
};

#include "sphere.cc"

#endif // _sphere_h_
