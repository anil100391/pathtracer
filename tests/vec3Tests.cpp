#include "../vec3.h"
#include <cassert>

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
using vec3f = vec3<float>;

int main()
{
    vec3f a{1.0f, 2.0f, 3.0f};
    vec3f b{4.0f, 3.0f, 9.0f};

    assert( a + b == vec3f(5.0f, 5.0f, 12.0f) );
    assert( a - b == vec3f(-3.0f, -1.0f, -6.0f) );
    assert( a % b == 37.0f );
    assert( a * b == vec3f(9.0f, 3.0f, -5.0f) );
    assert( a * 3.0f == vec3f(3.0f, 6.0f, 9.0f) );
    assert( vec3f( 2.0f, 4.0f, 8.0f ) / 2.0f == vec3f(1.0f, 2.0f, 4.0f) );

    assert( a.len2() == 14.0f );
    assert( a.len() == std::sqrt( 14.0f ) );

    vec3f c{5.0f, 10.0f, 12.0f};
    c -= {1.0f, 2.0f, -1.0f};
    assert( c == vec3f(4.0f, 8.0f, 13.0f) );
    c += {1.0f, 2.0f, -1.0f};
    assert( c == vec3f(5.0f, 10.0f, 12.0f) );

    c.normalize();
    assert( std::abs(c.len2() - 1.0f) < 1e-6 );
    std::cout << "All tests passed\n";
    return 0;
}
