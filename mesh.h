#pragma once
#include "boundingbox.h"
#include "hit.h"
#include "material.h"
#include "primitive.h"
#include "ray.h"
#include "mat44.h"
#include <vector>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class mesh : public primitive<T>
{
public:
    constexpr mesh() noexcept = default;
    mesh( const std::string &filename ) noexcept;

    virtual bool intersect( const ray<T> &r, hit<T> &h ) const
        noexcept override;

    constexpr void transform(const mat44<T> &mat) noexcept;

private:
    static bool intersect( const bbox<T> &box, const ray<T> &r );
    static bool rayTriaIntersect( const ray<T> & r,
                                  const vec3<T> &v0,
                                  const vec3<T> &v1,
                                  const vec3<T> &v2,
                                  hit<T> &       h );

    std::vector<vec3<T>>      _vertices;
    std::vector<unsigned int> _trias;

    bbox<T>  _box;
    std::vector<material> _mat;
};

#include "mesh.cc"
