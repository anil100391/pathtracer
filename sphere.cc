#include "sphere.h"
#include <algorithm>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
static bool solveQuadratic(const T &a, const T &b, const T &c, T &x0, T &x1) 
{ 
    T discr = b * b - 4 * a * c; 
    if (discr < 0) return false; 
    else if (discr == 0) x0 = x1 = - 0.5 * b / a; 
    else { 
        T q = (b > 0) ? 
            -0.5 * (b + std::sqrt(discr)) : 
            -0.5 * (b - std::sqrt(discr)); 
        x0 = q / a; 
        x1 = c / q; 
    } 
    if (x0 > x1) std::swap(x0, x1); 
 
    return true; 
} 

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
constexpr bool sphere<T>::intersect(const ray<T> &r, hit<T> &h) const noexcept
{
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection
    auto L = r.o - _center; 
    T a = r.o % r.o;
    T b = 2 * (r.o % L); 
    T c = L % L - _radius * _radius; 

    T t0, t1;
    if (!solveQuadratic(a, b, c, t0, t1)) return false; 
    if (t0 > t1) std::swap(t0, t1); 

    if (t0 < 0) { 
        t0 = t1; // if t0 is negative, let's use t1 instead 
        if (t0 < 0) return false; // both t0 and t1 are negative 
    } 

    h._pos = r.o + r.d * t0;

    return true; 
}