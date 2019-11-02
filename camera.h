#pragma once

#include "vec3.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class camera
{
public:
    constexpr camera( const vec3<T> &position, const vec3<T> &lookAt, float fov = 70 ) noexcept
        : _position( position ), _upVector( {0, 0, 1} ), _lookAt( lookAt ),
          _fov( .5 / tanf( fov * M_PI * .5f / 180.0 ) )
    {
        _dir = ( _lookAt - _position );
        _dir.normalize();
        _uVec = _dir * _upVector;
        _uVec.normalize();
        _vVec = _uVec * _dir;
        _vVec.normalize();
    }

    constexpr const vec3<T> &position() const noexcept { return _position; }
    constexpr const vec3<T> &lookAt() const noexcept { return _lookAt; }

    vec3<T> direction( T u, T v ) const noexcept
    {
        return ( _uVec * u + _vVec * v + _dir * _fov );
    }

private:
    vec3<T> _position;
    vec3<T> _upVector;
    vec3<T> _lookAt;
    float   _fov;

    // computed once when camera is initialized or when camera params change
    // used for making ray directions
    vec3<T> _dir;
    vec3<T> _uVec;
    vec3<T> _vVec;
};