/* Additional material
   -
*/

#include "tree_other/segment.hpp"
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>

template <typename T>
struct max {
    T operator()(const T& a, const T& b) const {
        return std::max(a, b);
    }
};
using tree_max_int = tree_other::segment<int, max>;
using tree_plus_int = tree_other::segment<int, std::plus>;
using tree_plus_str = tree_other::segment<std::string, std::plus>;
using tree_mul_int = tree_other::segment<int, std::multiplies>;
using tree_mod_int = tree_other::segment<int, std::modulus>;
using tree_xor_int = tree_other::segment<int, std::bit_xor>;

int main()
{
    std::cout << "SEGMENT TREE IMPLEMENTATION TESTS" << std::endl;
    // lval/rval, construction, border cases
    {
        std::cout << "checking usability ...";

        // various construction
        {
            auto v = { 1, 3, 5, 7, 9, 11 };
            tree_plus_int b1{ v };
            tree_plus_int b2{ 1, 3, 5, 7, 9, 11 };
            tree_plus_int b3{ v.begin(), v.end() };

            assert(query(b1, std::make_pair(0, 5)) == 36);
            assert(query(b2, std::make_pair(0, 5)) == 36);
            assert(query(b3, std::make_pair(0, 5)) == 36);

            tree_plus_int b4{};
            assert(query(b4, std::make_pair(0, 5)) == 0); // Note that wider bounds than the tree itself do not lead to crash
        }
        // query lval/rval
        {
            tree_plus_int b{ 1, 3, 5, 7, 9, 11 };
            auto i = std::make_pair(0, 5);
            assert(query(b, i) == 36);
            assert(query(b, std::make_pair(0, 5)) == 36);
        }
        // update lval/rval
        {
            tree_plus_int b{ 1, 3, 5, 7, 9, 11 };
            auto i1 = std::make_pair(1, 1);
            auto s1 = 30;
            update(b, i1, s1);
            update(b, std::make_pair(2, 2), s1);
            update(b, std::make_pair(3, 3), 30);
            assert(query(b, std::make_pair(1, 1)) == 30);
            assert(query(b, std::make_pair(2, 2)) == 30);
            assert(query(b, std::make_pair(3, 3)) == 30);
        }
        std::cout << "ok" << std::endl;
    }
    // query
    {
        std::cout << "checking query ...";
        tree_plus_int b1{ 1, 3, 5, 7, 9, 11 };
        assert(query(b1, std::make_pair(2, 4)) == 21);

        tree_max_int b2{ 1, 3, 5, 7, 9, 11 };
        assert(query(b2, std::make_pair(0, 3)) == 7);
        assert(query(b2, std::make_pair(2, 4)) == 9);

        tree_plus_str b3{ {"aa", "bb", "cc", "dd", "ee", "ff"} };
        assert(query(b3, std::make_pair(0, 6)) == "aabbccddeeff");
        assert(query(b3, std::make_pair(2, 4)) == "ccddee");
        assert(query(b3, std::make_pair(0, 7)) == "aabbccddeeff");

        tree_mul_int b4{ 1, 3, 5, 7, 9, 11 };
        assert(query(b4, std::make_pair(0, 5)) == 10395);

        tree_mod_int b5{ 1, 3, 5, 7, 9, 11 };
        assert(query(b5, std::make_pair(0, 5)) == 1);
        assert(query(b5, std::make_pair(1, 3)) == 3);

        tree_xor_int b6{ 1, 3, 5, 7, 9, 11 };
        assert(query(b6, std::make_pair(0, 5)) == 2);
        assert(query(b6, std::make_pair(1, 3)) == 1);
        std::cout << "ok" << std::endl;
    }
    // update
    {
        std::cout << "checking update ...";
        tree_plus_int b1{ 1, 3, 5, 7, 9, 11 };
        update(b1, std::make_pair(1, 1), 10);
        assert(query(b1, std::make_pair(0, 2)) == 16);

        tree_max_int b2{ 1, 3, 5, 7, 9, 11 };
        update(b2, std::make_pair(1, 1), 10);
        assert(query(b2, std::make_pair(0, 4)) == 10);
        assert(query(b2, std::make_pair(0, 5)) == 11);
        assert(query(b2, std::make_pair(2, 4)) == 9);

        tree_plus_int b3{ 1, 3, 5, 7, 9, 11 };
        update(b3, std::make_pair(1, 3), 10);
        assert(query(b3, std::make_pair(0, 4)) == 40);

        tree_plus_str b4{ {"aa", "bb", "cc", "dd", "ee", "ff"} };
        update(b4, std::make_pair(1, 3), std::string("k1"));
        assert(query(b4, std::make_pair(0, 6)) == "aak1k1k1eeff");
        assert(query(b4, std::make_pair(1, 2)) == "k1k1");
        std::cout << "ok" << std::endl;
    }
}
