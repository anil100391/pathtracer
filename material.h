#ifndef _material_h_
#define _material_h_

#include "vec3.h"
#include <algorithm>

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

    void clamp()
    {
        r = std::clamp( r, 0.0f, 1.0f );
        g = std::clamp( g, 0.0f, 1.0f );
        b = std::clamp( b, 0.0f, 1.0f );
        a = std::clamp( a, 0.0f, 1.0f );
    }

    vec3<unsigned char> touchar() noexcept
    {
        clamp();
        return {
            static_cast<unsigned char>( 255 * std::pow( r, 1 / 2.2f ) + 0.5f ),
            static_cast<unsigned char>( 255 * std::pow( g, 1 / 2.2f ) + 0.5f ),
            static_cast<unsigned char>( 255 * std::pow( b, 1 / 2.2f ) + 0.5f )};
    }

    static color random()
    {
        return {1.0f * rand() / RAND_MAX,
                1.0f * rand() / RAND_MAX,
                1.0f * rand() / RAND_MAX,
                1.0f};
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
