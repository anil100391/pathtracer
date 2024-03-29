#ifndef _primitive_h_
#define _primitive_h_

#include "hit.h"
#include "ray.h"

template <typename T>
class primitive
{
public:
    virtual bool intersect( const ray<T> &r, hit<T> &h ) const noexcept
    {
        return false;
    }
};

#endif // _primitive_h_
