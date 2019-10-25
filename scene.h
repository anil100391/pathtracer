#ifndef _scene_h_
#define _scene_h_

#include <limits>
#include <vector>
#include "hit.h"
#include "primitive.h"

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
template <typename T>
class scene
{
public:
    constexpr scene() noexcept = default;

    scene& operator << (primitive<T> *p) noexcept
    {
        _primitives.push_back(p);
        return *this;
    }

    constexpr bool intersect(const ray<T> &r, hit<T> &h) const noexcept
    {
        hit<T> curHit;
        T dist = std::numeric_limits<T>::infinity();
        for ( auto& p : _primitives)
        {
            if ( !p->intersect(r, curHit))
                continue;

            T curDist = (curHit._pos - r.o).len2();
            if ( curDist < dist )
            {
                dist = curDist;
                h = curHit;
            }
            if ( dist != std::numeric_limits<T>::infinity() )
                return true;
        }
        return false;
    }

private:

    std::vector<primitive<T> *>  _primitives;
};

#endif // _scene_h_
