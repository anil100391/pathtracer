#include "sphere.h"
#include <algorithm>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
static bool solveQuadratic( const T &a, const T &b, const T &c, T &x0, T &x1 )
{
    T discr = b * b - 4 * a * c;
    if ( discr < 0 )
        return false;
    else if ( discr == 0 )
        x0 = x1 = -0.5 * b / a;
    else
    {
        T q = ( b > 0 ) ? -0.5 * ( b + std::sqrt( discr ) )
                        : -0.5 * ( b - std::sqrt( discr ) );
        x0 = q / a;
        x1 = c / q;
    }
    if ( x0 > x1 )
        std::swap( x0, x1 );

    return true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
constexpr bool sphere<T>::intersect( const ray<T> &r, hit<T> &h ) const noexcept
{
    auto c = _center - r.o;
    T csq = c.len2();

    T b = c % r.d;

    T asq = csq - b * b;

    if ( asq > _radius * _radius )
        return false;

    T trappedSegHalf = std::sqrt(_radius * _radius - asq);
    h._pos = r.o + r.d * (b - trappedSegHalf);
    h._normal = h._pos - _center;
    h._normal.normalize();
    h._mat = _mat;
    return true;
}
