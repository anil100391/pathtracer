#pragma once
#include "boundingbox.h"
#include "hit.h"
#include "material.h"
#include "primitive.h"
#include "ray.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class mesh : public primitive<T>
{
public:
    constexpr mesh() noexcept = default;
    constexpr mesh( const std::string &filename ) noexcept;

    constexpr virtual bool intersect( const ray<T> &r, hit<T> &h ) const
        noexcept override;

    constexpr const material &getMaterial() const noexcept { return _mat; }

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
    material _mat;
};

#include "mesh.cc"
