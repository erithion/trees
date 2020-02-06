/* Additional material
   -
*/

#include "tree_other/segment.hpp"
#include <iostream>
#include <cassert>
#include <functional>
#include <algorithm>
#include <limits>

//using k = std::integral_constant< std::decay_t<decltype(std::max<int>(0, 0))>, std::max<int>>;

using tree = tree_other::segment<int, std::plus<int>>;
//using tree_max = tree_other::segment<int, std::integral_constant<std::decay_t<decltype(std::max<int>)>, std::max<int>>> ; // make work
template <typename T>
struct max {
    using result_type = T;
    static constexpr result_type identity = std::numeric_limits<T>::min();
    result_type operator()(const T& a, const T& b) const {
        return std::max(a, b);
    }
};
using tree_max = tree_other::segment<int, max<int>>;

int main()
{
    // TODO: better test cases
    std::cout << "SEGMENT TREE IMPLEMENTATION TESTS" << std::endl;
    {
        // must fail for now
        using tree_to_fail = tree_other::segment<std::string, std::plus<std::string>>;
//        tree_to_fail t;
    }
    // edge cases
    {
        std::cout << "checking edge cases ...";
        tree bit{};
        assert(bit.vec_ == std::vector<int>(0));
        std::cout << "ok" << std::endl;
    }
    // simple
    {
        std::cout << "checking a trivial case (sum) ...";
        tree bit{ 1, 2, 3, 4, 5 };
        assert(bit.vec_ == std::vector<int>({15, 3, 12, 1, 2, 3, 9, 0, 0, 0, 0, 0, 0, 4, 5, 0, 0, 0, 0, 0}));
        std::cout << "ok" << std::endl;
    }
    // hackerearth example
    {
        std::cout << "checking cretion ...";
        tree bit{ 1, 3, 5, 7, 9, 11 };
        assert(bit.vec_ == std::vector<int>({ 36, 9, 27, 1, 8, 7, 20, 0, 0, 3, 5, 0, 0, 9, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0 }));
        std::cout << "ok" << std::endl;
    }
    // hackerearth query
    {
        std::cout << "checking query ...";
        tree bit{ 1, 3, 5, 7, 9, 11 };
        assert(query(bit, 2, 4) == 21);
        std::cout << "ok" << std::endl;
    }
    // test max
    {
        std::cout << "checking query (max) ...";
        tree_max bit{ 1, 3, 5, 7, 9, 11 };
        assert(query(bit, 0, 3) == 7);
        assert(query(bit, 2, 4) == 9);
        std::cout << "ok" << std::endl;
    }
    // test update
    {
        std::cout << "checking update ...";
        tree bit{ 1, 3, 5, 7, 9, 11 };
        update(bit, std::make_pair(1, 1), 10);
        assert(bit.vec_ == std::vector<int>({ 43, 16, 27, 1, 15, 7, 20, 0, 0, 10, 5, 0, 0, 9, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0 }));
        std::cout << "ok" << std::endl;
    }
    // test max update
    {
        std::cout << "checking update (max) ...";
        tree_max bit{ 1, 3, 5, 7, 9, 11 };
        update(bit, std::make_pair(1, 1), 10);
        assert(query(bit, 0, 4) == 10);
        assert(query(bit, 0, 5) == 11);
        assert(query(bit, 2, 4) == 9);
        std::cout << "ok" << std::endl;
    }
    // test interval update
    {
        std::cout << "checking update multiple ...";
        tree bit{ 1, 3, 5, 7, 9, 11 };
        update(bit, std::make_pair(1, 3), 10);
        assert(bit.vec_ == std::vector<int>({ 51, 21, 30, 1, 20, 10, 20, 0, 0, 10, 10, 0, 0, 9, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0 }));
        std::cout << "ok" << std::endl;
    }
}
