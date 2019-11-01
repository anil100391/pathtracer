#include "../boundingbox.h"
#include <iostream>

using FLOAT = float;
int main()
{
    {
        bbox<FLOAT> box{};
        assert( !box.isvalid() );
        std::cout << box << "\n";
        box.expand({0, 0, 0});
        std::cout << box << "\n";
        box.expand({1, 2, 3});
        std::cout << box << "\n";
        assert( box.isvalid() );
        assert( box.contains( {0, 0, 0} ) );
        assert( !box.contains( {1, 2, 3} ) );
        assert( !box.contains( {1, 0, 0} ) );
        assert( box.contains( {0.5, 0.5, 0.5} ) );
    }

    {
        bbox<FLOAT> box{{0, 0, 0}, {1, 2, 3}};
        assert( box.isvalid() );
        assert( box.contains( {0, 0, 0} ) );
        assert( !box.contains( {1, 2, 3} ) );
        assert( !box.contains( {1, 0, 0} ) );
        assert( box.contains( {0.5, 0.5, 0.5} ) );
        std::cout << box << "\n";
    }
    return 0;
}
