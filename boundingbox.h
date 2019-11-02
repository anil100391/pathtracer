#pragma once
#include "vec3.h"
#include <cassert>
#include <limits>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class bbox
{
public:
    constexpr bbox() noexcept = default;
    constexpr bbox( const vec3<T> &min, const vec3<T> &max ) noexcept
        : _min( min ), _max( max )
    {
    }

    constexpr const vec3<T> &min() const noexcept { return _min; }
    constexpr const vec3<T> &max() const noexcept { return _max; }

    constexpr bool contains( const vec3<T> &point ) const noexcept
    {
        assert( isvalid() );
        return point[0] >= _min[0] && point[0] < _max[0] &&
               point[1] >= _min[1] && point[1] < _max[1] &&
               point[2] >= _min[2] && point[2] < _max[2];
    }

    constexpr bool isvalid() const noexcept
    {
        return _min[0] <= _max[0] && _min[1] <= _max[1] && _min[2] <= _max[2];
    }

    constexpr void reset() noexcept
    {
        _min = {std::numeric_limits<T>::max(),
                std::numeric_limits<T>::max(),
                std::numeric_limits<T>::max()};
        _max = {std::numeric_limits<T>::lowest(),
                std::numeric_limits<T>::lowest(),
                std::numeric_limits<T>::lowest()};
    }

    constexpr void expand( const vec3<T> &point ) noexcept
    {
        if ( point[0] < _min[0] )
            _min[0] = point[0];
        if ( point[1] < _min[1] )
            _min[1] = point[1];
        if ( point[2] < _min[2] )
            _min[2] = point[2];
        if ( point[0] > _max[0] )
            _max[0] = point[0];
        if ( point[1] > _max[1] )
            _max[1] = point[1];
        if ( point[2] > _max[2] )
            _max[2] = point[2];
    }

    T radius() const noexcept
    {
        vec3<T> c = center();
        return ( _max - c ).len();
    }

    constexpr vec3<T> center() const noexcept { return ( _min + _max ) / 2.0f; }

    friend std::ostream &operator<<( std::ostream &os, const bbox<float> &box );
    friend std::ostream &operator<<( std::ostream &      os,
                                     const bbox<double> &box );

private:
    vec3<T> _min{std::numeric_limits<T>::max(),
                 std::numeric_limits<T>::max(),
                 std::numeric_limits<T>::max()};
    vec3<T> _max{std::numeric_limits<T>::lowest(),
                 std::numeric_limits<T>::lowest(),
                 std::numeric_limits<T>::lowest()};
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::ostream &operator<<( std::ostream &os, const bbox<float> &box )
{
    os << "min: (" << box.min() << ") | max: (" << box.max() << ")";
    return os;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
std::ostream &operator<<( std::ostream &os, const bbox<double> &box )
{
    os << "min: (" << box.min() << ") | max: (" << box.max() << ")";
    return os;
}
