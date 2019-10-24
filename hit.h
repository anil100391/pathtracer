#ifndef _hit_h_
#define _hit_h_

#include "material.h"
#include "vec3.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template <typename T>
class hit
{
public:
    hit() = default;
    vec3<T>  _pos;
    vec3<T>  _normal;
    material _mat;
};

#endif // _hit_h_
