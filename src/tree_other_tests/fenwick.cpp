/* Additional material
   - https://www.youtube.com/watch?v=RgITNht_f4Q
*/

#include "tree_other/fenwick.hpp"
#include <iostream>
#include <cassert>

int main()
{
    // testing creation and getters
    // 1 2 3 4 5  -> 1 3 6 10 15
    // 1 2 3 4 5     1 2 3  4  5
    {
        const tree_other::fenwick<int> bit{ 1, 2, 3, 4, 5 };
        assert(bit[1] == 1);
        assert(bit[2] == 3);
        assert(bit[3] == 6);
        assert(bit[4] == 10);
        assert(bit[5] == 15);
    }
    // testing updates
    // 1 2 3 4 5 +-> 10 -> 1 3 16 20 25
    // 1 2 3 4 5 +->  3 -> 1 2 3  4  5
    {
        tree_other::fenwick<int> bit{ 1, 2, 3, 4, 5 };
        bit[3] += 10;
        assert(bit[1] == 1);
        assert(bit[2] == 3);
        assert(bit[3] == 16);
        assert(bit[4] == 20);
        assert(bit[5] == 25);
    }
    // anything except arithmetic types are not supported
    {
//       BinaryIndexedTree<std::string> bit{ "1232", "sd", "fd" };
    }
    // testing the range case
    {
        tree_other::fenwick<int> bit{ 1, 2, 3, 4, 5 };
        assert(bit.range(4, 5) == 9);
    }
    // testing the range edge cases
    {
        tree_other::fenwick<int> bit{ 1, 2, 3, 4, 5 };
        assert(bit.range(1, 1) == 1);
        assert(bit.range(5, 5) == 5);
    }
}
