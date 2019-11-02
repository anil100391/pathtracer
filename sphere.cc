#include "sphere.h"
#include <algorithm>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
bool sphere<T>::intersect( const ray<T> &r, hit<T> &h ) const noexcept
{
    // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
    auto op            = _center - r.o;
    auto radiusSquared = _radius * _radius;
    auto b             = op % r.d;
    auto determinant   = b * b - op.len2() + radiusSquared;
    if ( determinant < 0 )
        return false;

    determinant              = std::sqrt( determinant );
    auto             minusT  = b - determinant;
    auto             plusT   = b + determinant;
    constexpr double Epsilon = 0.000000001;
    if ( minusT < Epsilon && plusT < Epsilon )
        return false;

    auto t           = minusT > Epsilon ? minusT : plusT;
    auto hitPosition = r.o + r.d * t;
    auto normal      = ( hitPosition - _center );
    normal.normalize();
    bool inside = normal % r.d > 0;
    if ( inside )
        normal = normal * -1.0f;
    h._pos    = hitPosition;
    h._normal = normal;
    h._mat    = _mat;
    return true;
}
