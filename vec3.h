#ifndef _vec3_h_
#define _vec3_h_

#include <cmath>
#include <cstdlib>
#include <iosfwd>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class vec3
{
public:
    constexpr vec3() noexcept = default;

    constexpr vec3( T a, T b, T c ) noexcept
    {
        _x[0] = a;
        _x[1] = b;
        _x[2] = c;
    }

    constexpr explicit vec3( const T *x ) noexcept
    {
        memcpy( _x, x, 3 * sizeof( T ) );
    }

    // operators
    constexpr vec3<T> operator+( const vec3<T> &other ) const noexcept
    {
        return {_x[0] + other._x[0], _x[1] + other._x[1], _x[2] + other._x[2]};
    }

    constexpr vec3<T> operator-( const vec3<T> &other ) const noexcept
    {
        return {_x[0] - other._x[0], _x[1] - other._x[1], _x[2] - other._x[2]};
    }

    constexpr vec3<T> &operator+=( const vec3<T> &other ) noexcept
    {
        _x[0] += other._x[0];
        _x[1] += other._x[1];
        _x[2] += other._x[2];
        return *this;
    }

    constexpr vec3<T> &operator-=( const vec3<T> &other ) noexcept
    {
        _x[0] -= other._x[0];
        _x[1] -= other._x[1];
        _x[2] -= other._x[2];
        return *this;
    }

    constexpr vec3<T> operator*( const vec3<T> &other ) const noexcept
    {
        return {_x[1] * other._x[2] - other._x[1] * _x[2],
                -_x[0] * other._x[2] + other._x[0] * _x[2],
                _x[0] * other._x[1] - other._x[0] * _x[1]};
    }

    constexpr T operator%( const vec3<T> &other ) const noexcept
    {
        return _x[0] * other._x[0] + _x[1] * other._x[1] + _x[2] * other._x[2];
    }

    constexpr T len2() const noexcept
    {
        return _x[0] * _x[0] + _x[1] * _x[1] + _x[2] * _x[2];
    }

    T len() const noexcept
    {
        return std::sqrt( _x[0] * _x[0] + _x[1] * _x[1] + _x[2] * _x[2] );
    }

    constexpr vec3<T> operator*( T scale ) const noexcept
    {
        return {_x[0] * scale, _x[1] * scale, _x[2] * scale};
    }

    constexpr vec3<T> operator/( T div ) const noexcept
    {
        return {_x[0] / div, _x[1] / div, _x[2] / div};
    }

    constexpr vec3<T>& operator/=( T div ) noexcept
    {
        _x[0] /= div; _x[1] /= div; _x[2] /= div;
        return *this;
    }

    T normalize() noexcept
    {
        T l = len();
        if ( l > T( 0 ) )
        {
            _x[0] /= l;
            _x[1] /= l;
            _x[2] /= l;
        }

        return l;
    }

    constexpr const T &operator[]( unsigned int index ) const noexcept
    {
        return _x[index];
    }

    constexpr T &operator[]( unsigned int index ) noexcept { return _x[index]; }

    constexpr bool operator==( const vec3<T> &other ) const noexcept
    {
        return _x[0] == other._x[0] && _x[1] == other._x[1] &&
               _x[2] == other._x[2];
    }

    friend std::ostream &operator<<( std::ostream &os, const vec3<float> &vec );
    friend std::ostream &operator<<( std::ostream &      os,
                                     const vec3<double> &vec );

private:
    T _x[3] = {T( 0 ), T( 0 ), T( 0 )};
};

#include "vec3.cc"

#endif // _vec3_h_
