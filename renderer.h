#pragma once

#include "camera.h"
#include "scene.h"
#include "material.h"
#include <random>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
struct renderparams
{
    constexpr unsigned int width() const noexcept { return _width; }
    constexpr unsigned int height() const noexcept { return _height; }
    constexpr unsigned int maxDepth() const noexcept { return _maxDepth; }
    constexpr unsigned int numSamples() const noexcept { return _numSamples; }

    unsigned int _width      = 512;
    unsigned int _height     = 512;
    unsigned int _maxDepth   = 4;
    unsigned int _numSamples = 512;
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class renderer
{
public:
    renderer( const camera<T> &cam, const renderparams &rp )
        : _camera( cam ), _renderParams( rp )
    {
    }

    void render( const scene<T> &world );

private:
    color               tracepath( const scene<T> &world,
                                   std::mt19937 &  rng,
                                   const ray<T> &  r,
                                   unsigned int    depth );
    const camera<T> &   _camera;
    const renderparams &_renderParams;
};

#include "renderer.cc"
