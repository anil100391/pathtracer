#pragma once

#include <array>
#include <cmath>
#include <cstdlib>
#include <iosfwd>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class mat44
{
public:
    static mat44<T> makeIdentity()
    {
        return { T(1), T(0), T(0), T(0),
                 T(0), T(1), T(0), T(0),
                 T(0), T(0), T(1), T(0),
                 T(0), T(0), T(0), T(1) };
    }

    static mat44<T> makeRotation( float deg, int axis )
    {
        float rad = deg * M_PI / 180.0f;
        float cos = std::cos( rad );
        float sin = std::sin( rad );
        switch ( axis )
        {
        case 0:
            return { T(1), T(0), T(0), T(0),
                     T(0), cos,  -sin, T(0),
                     T(0), sin,   cos, T(0),
                     T(0), T(0), T(0), T(1) };
        case 1:
            return { cos,  T(0), sin, T(0),
                     T(0), T(1), T(0), T(0),
                     -sin, T(0), cos,  T(0),
                     T(0), T(0), T(0), T(1) };
        case 2:
            return { cos,  -sin, T(0), T(0),
                     sin,   cos, T(0), T(0),
                     T(0), T(0), T(1), T(0),
                     T(0), T(0), T(0), T(1) };
        }
        assert( false );
        return makeIdentity();
    }

    static mat44<T> makeScale( float scale )
    {
        return { scale, T(0),  T(0),  T(0),
                 T(0),  scale, T(0),  T(0),
                 T(0),  T(0),  scale, T(0),
                 T(0),  T(0),  T(0),  T(1) };
    }

    constexpr mat44() noexcept = default;

    constexpr explicit mat44( const T *vals ) noexcept
    {
        memcpy( _x, vals, 16 * sizeof( T ) );
    }

    constexpr mat44( T a00, T a01, T a02, T a03,
                     T a10, T a11, T a12, T a13,
                     T a20, T a21, T a22, T a23,
                     T a30, T a31, T a32, T a33 ) noexcept
    {
        _x[0]  = a00; _x[1]  = a01; _x[2]  = a02; _x[3]  = a03;
        _x[4]  = a10; _x[5]  = a11; _x[6]  = a12; _x[7]  = a13;
        _x[8]  = a20; _x[9]  = a21; _x[10] = a22; _x[11] = a23;
        _x[12] = a30; _x[13] = a31; _x[14] = a32; _x[15] = a33;
    }

    constexpr void Transform( vec3<T> &v ) const noexcept
    {
        vec3<T> in = v;
        T scale = _x[12] * in[0] + _x[13] * in[1] + _x[14] * in[2] + _x[15];
        v[0] = _x[0] * in[0] + _x[1] * in[1] + _x[2] * in[2] + _x[3];
        v[1] = _x[4] * in[0] + _x[5] * in[1] + _x[6] * in[2] + _x[7];
        v[2] = _x[8] * in[0] + _x[9] * in[1] + _x[10] * in[2] + _x[11];
        v /= scale;
    }

    constexpr void Transform33( vec3<T> &v ) const noexcept
    {
        vec3<T> in = v;
        v[0] = _x[0] * in[0] + _x[1] * in[1] + _x[2] * in[2];
        v[1] = _x[4] * in[0] + _x[5] * in[1] + _x[6] * in[2];
        v[2] = _x[8] * in[0] + _x[9] * in[1] + _x[10] * in[2];
    }

    friend std::ostream &operator<<( std::ostream &      os,
                                     const mat44<float> &vec );
    friend std::ostream &operator<<( std::ostream &       os,
                                     const mat44<double> &vec );

private:
    std::array<T, 16> _x = {T( 0 )};
};

#include "mat44.cc"
