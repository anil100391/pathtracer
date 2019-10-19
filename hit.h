#ifndef _hit_h_
#define _hit_h_

#include "vec3.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
template<typename T>
class hit
{
public:
    vec3<T> _pos;
    vec3<T> _normal;
};

#endif // _hit_h_
