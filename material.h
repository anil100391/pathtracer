#ifndef _material_h_
#define _material_h_

#include "vec3.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct color
{
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 1.0f;

    color operator/( float scale ) const
    {
        return {r / scale, g / scale, b / scale, a / scale};
    }

    color operator*( float scale ) const
    {
        return {r * scale, g * scale, b * scale, a * scale};
    }

    color operator+( const color &other ) const
    {
        return {r + other.r, g + other.g, b + other.b, a + other.a};
    }

    color operator*( const color &other ) const
    {
        return {r * other.r, g * other.g, b * other.b, a * other.a};
    }

    void clip()
    {
        if ( r < 0 )
            r = 0;
        if ( g < 0 )
            g = 0;
        if ( b < 0 )
            b = 0;
        if ( a < 0 )
            a = 0;

        if ( r > 1 )
            r = 1;
        if ( g > 1 )
            g = 1;
        if ( b > 1 )
            b = 1;
        if ( a > 1 )
            a = 1;
    }
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
class material
{
public:
    constexpr material() noexcept = default;
    constexpr material(
        const color &diffuse,
        const color &specular = {1.0f, 1.0f, 1.0f, 1.0f} ) noexcept
        : _diffuse( diffuse ), _specular( specular )
    {
    }

    constexpr const color &diffuse() const noexcept { return _diffuse; }
    constexpr const color &specular() const noexcept { return _specular; }
    constexpr float        emission() const noexcept { return _emission; }

    constexpr void setEmissive( float strength ) noexcept
    {
        _emission = strength;
    }

private:
    color _diffuse  = {1.0f, 1.0f, 1.0f, 1.0f};
    color _specular = {1.0f, 1.0f, 1.0f, 1.0f};
    float _emission = 0.0f;
};

#endif // _material_h_
