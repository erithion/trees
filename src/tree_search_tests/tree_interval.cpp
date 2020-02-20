/* Additional material
*/

#include "tree_search/tree_interval.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_search_interval.hpp"
#include "tree_search/capability_search.hpp"
#include "tree_search/tree.hpp"
#include <cassert>
#include <iostream>
#include <array>
#include <vector>

namespace ts = tree_search;

template <typename T>
using tree = ts::tree<std::pair< T, T>, ts::aux::interval_augment<T>, ts::capability_traverse, ts::capability_insert_interval, ts::capability_search_interval>;

// Must fail to compile: capability_search_interval works only when interval_augment type has also been specified
//template <typename T>
//using tree = ts::tree<std::pair< T, T>, ts::aux::empty, ts::capability_traverse, ts::capability_insert_interval, ts::capability_search_interval>;

void check_insert() {

    std::cout << "checking insertion ...";
    tree<int> bin;
    insert(bin, std::make_pair(1, 10)); // rvalue
    insert(bin, { std::make_pair(-7, 3), std::make_pair(-10, 20) }); // init list
    auto vv = std::make_pair(17, 35);
    insert(bin, vv); // lvalue
    assert(ts::aux::access(bin)->value_ == std::make_pair(-7, 3));
    assert(ts::aux::access(bin)->left_->value_ == std::make_pair(-10, 20));
    assert(ts::aux::access(bin)->right_->value_ == std::make_pair(1, 10));
    assert(ts::aux::access(bin)->right_->right_->value_ == std::make_pair(17, 35));
    std::cout << "ok" << std::endl;
}

void check_traversing() {

    std::cout << "checking traversing ...";
    tree<int> bin;
    insert(bin
        , { std::make_pair(15, 20)
          , std::make_pair(10, 30)
          , std::make_pair(17, 19)
          , std::make_pair(5, 20)
          , std::make_pair(12, 15)
          , std::make_pair(30, 40) });
    // preorder
    {
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(15, 20)
                                                , std::make_pair(10, 30)
                                                , std::make_pair(5, 20)
                                                , std::make_pair(12, 15)
                                                , std::make_pair(17, 19)
                                                , std::make_pair(30, 40) };
        assert(res == truth);
    }
    // inorder
    {
        auto it = traverse(ts::tag_inorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(5, 20)
                                                , std::make_pair(10, 30)
                                                , std::make_pair(12, 15)
                                                , std::make_pair(15, 20)
                                                , std::make_pair(17, 19)
                                                , std::make_pair(30, 40) };
        assert(res == truth);
    }
    // postorder
    {
        auto it = traverse(ts::tag_postorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(5, 20)
                                                , std::make_pair(12, 15)
                                                , std::make_pair(10, 30)
                                                , std::make_pair(30, 40)
                                                , std::make_pair(17, 19)
                                                , std::make_pair(15, 20) };
        assert(res == truth);
    }
    std::cout << "ok" << std::endl;
}

void check_search_iterator_invariants() {

    std::cout << "checking search iterator invariants ...";
    tree<int> bin;
    insert(bin
        , { std::make_pair(15, 20)
          , std::make_pair(10, 30) });
    // preorder
    {
        // range loop
        {
            std::vector<tree<int>::value_type> res = {};
            for (auto v : search(ts::tag_preorder{}, bin, std::make_pair(17, 22))) res.emplace_back(v);
            std::vector<tree<int>::value_type> truth{ std::make_pair(15, 20), std::make_pair(10, 30) };
            assert(res == truth);
        }
        // begin/end
        {
            auto it = search(ts::tag_preorder{}, bin, std::make_pair(17, 22));
            std::vector<tree<int>::value_type> res = {it.begin_, it.end_};
            std::vector<tree<int>::value_type> truth{ std::make_pair(15, 20), std::make_pair(10, 30) };
            assert(res == truth);
        }
        // prefix++
        {
            auto it = search(ts::tag_preorder{}, bin, std::make_pair(17, 22));
            assert(*it.begin_ == std::make_pair(15, 20));
            assert(*++it.begin_ == std::make_pair(10, 30));
            assert(++it.begin_ == it.end_);
        }
        // postfix++
        {
            auto it = search(ts::tag_preorder{}, bin, std::make_pair(17, 22));
            assert(*it.begin_++ == std::make_pair(15, 20));
            assert(*it.begin_++ == std::make_pair(10, 30));
            assert(it.begin_ == it.end_);
        }
        // const check
        {
            auto it = search(ts::tag_preorder{}, bin, std::make_pair(17, 22));
            static_assert (std::is_same_v<decltype(*it.begin_), const tree<int>::value_type&>, "preorder search iterator should return const reference");
        }
    }
    // inorder
    {
        // range loop
        {
            std::vector<tree<int>::value_type> res = {};
            for (auto v : search(ts::tag_inorder{}, bin, std::make_pair(17, 22))) res.emplace_back(v);
            std::vector<tree<int>::value_type> truth{ std::make_pair(10, 30), std::make_pair(15, 20) };
            assert(res == truth);
        }
        // begin/end
        {
            auto it = search(ts::tag_inorder{}, bin, std::make_pair(17, 22));
            std::vector<tree<int>::value_type> res = { it.begin_, it.end_ };
            std::vector<tree<int>::value_type> truth{ std::make_pair(10, 30), std::make_pair(15, 20) };
            assert(res == truth);
        }
        // prefix++
        {
            auto it = search(ts::tag_inorder{}, bin, std::make_pair(17, 22));
            assert(*it.begin_ == std::make_pair(10, 30));
            assert(*++it.begin_ == std::make_pair(15, 20));
            assert(++it.begin_ == it.end_);
        }
        // postfix++
        {
            auto it = search(ts::tag_inorder{}, bin, std::make_pair(17, 22));
            assert(*it.begin_++ == std::make_pair(10, 30));
            assert(*it.begin_++ == std::make_pair(15, 20));
            assert(it.begin_ == it.end_);
        }
        // const check
        {
            auto it = search(ts::tag_inorder{}, bin, std::make_pair(17, 22));
            static_assert (std::is_same_v<decltype(*it.begin_), const tree<int>::value_type&>, "inorder search iterator should return const reference");
        }
    }
    // postorder
    {
        // range loop
        {
            std::vector<tree<int>::value_type> res = {};
            for (auto v : search(ts::tag_postorder{}, bin, std::make_pair(17, 22))) res.emplace_back(v);
            std::vector<tree<int>::value_type> truth{ std::make_pair(10, 30), std::make_pair(15, 20) };
            assert(res == truth);
        }
        // begin/end
        {
            auto it = search(ts::tag_postorder{}, bin, std::make_pair(17, 22));
            std::vector<tree<int>::value_type> res = { it.begin_, it.end_ };
            std::vector<tree<int>::value_type> truth{ std::make_pair(10, 30), std::make_pair(15, 20) };
            assert(res == truth);
        }
        // prefix++
        {
            auto it = search(ts::tag_postorder{}, bin, std::make_pair(17, 22));
            assert(*it.begin_ == std::make_pair(10, 30));
            assert(*++it.begin_ == std::make_pair(15, 20));
            assert(++it.begin_ == it.end_);
        }
        // postfix++
        {
            auto it = search(ts::tag_postorder{}, bin, std::make_pair(17, 22));
            assert(*it.begin_++ == std::make_pair(10, 30));
            assert(*it.begin_++ == std::make_pair(15, 20));
            assert(it.begin_ == it.end_);
        }
        // const check
        {
            auto it = search(ts::tag_postorder{}, bin, std::make_pair(17, 22));
            static_assert (std::is_same_v<decltype(*it.begin_), const tree<int>::value_type&>, "postorder search iterator should return const reference");
        }
    }
    std::cout << "ok" << std::endl;
}

void check_search_geeks_example() {

    std::cout << "checking search (geeks) ...";
    tree<int> bin;
    insert(bin
        , { std::make_pair(15, 20)
          , std::make_pair(10, 30)
          , std::make_pair(17, 19)
          , std::make_pair(5, 20)
          , std::make_pair(12, 15)
          , std::make_pair(30, 40) });
    // preorder
    {
        auto it = search(ts::tag_preorder{}, bin, std::make_pair(12, 16));
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(15, 20)
                                                , std::make_pair(10, 30)
                                                , std::make_pair(5, 20)
                                                , std::make_pair(12, 15) };
        assert(res == truth);
    }
    // inorder
    {
        auto it = search(ts::tag_inorder{}, bin, std::make_pair(12, 16));
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(5, 20)
                                                , std::make_pair(10, 30)
                                                , std::make_pair(12, 15)
                                                , std::make_pair(15, 20) };
        assert(res == truth);
    }
    // postorder
    {
        auto it = search(ts::tag_postorder{}, bin, std::make_pair(12, 16));
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(5, 20)
                                                , std::make_pair(12, 15)
                                                , std::make_pair(10, 30)
                                                , std::make_pair(15, 20) };
        assert(res == truth);
    }
    std::cout << "ok" << std::endl;
}

void check_search_wiki_example() {

    std::cout << "checking search (wiki) ...";
    tree<int> bin;
    insert(bin
        , { std::make_pair(20, 36)
          , std::make_pair(3, 41)
          , std::make_pair(29, 99)
          , std::make_pair(0, 1)
          , std::make_pair(10, 15) });
    // preorder
    {
        auto it = search(ts::tag_preorder{}, bin, std::make_pair(35, 60));
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(20, 36)
                                                , std::make_pair(3, 41)
                                                , std::make_pair(29, 99) };
        assert(res == truth);
    }
    // inorder
    {
        auto it = search(ts::tag_inorder{}, bin, std::make_pair(35, 60));
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(3, 41)
                                                , std::make_pair(20, 36)
                                                , std::make_pair(29, 99) };
        assert(res == truth);
    }
    // postorder
    {
        auto it = search(ts::tag_postorder{}, bin, std::make_pair(35, 60));
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth{ std::make_pair(3, 41)
                                                , std::make_pair(29, 99)
                                                , std::make_pair(20, 36) };
        assert(res == truth);
    }
    std::cout << "ok" << std::endl;
}

void generic_search() {

    using tree2 = ts::tree<std::pair< int, int>, ts::aux::interval_augment<int>, ts::capability_traverse, ts::capability_insert_interval, ts::capability_search_interval, ts::capability_search>;

    tree2 bin;
    insert(bin
        , { std::make_pair(15, 20)
          , std::make_pair(10, 30)
          , std::make_pair(17, 19)
          , std::make_pair(5, 20)
          , std::make_pair(12, 15)
          , std::make_pair(30, 40) });
    // TODO: try implementing compile time failure checks 
    // must fail to compile. for generic search with pair of integers use plain tree.
    auto lambda = [](auto v) {return true; };
    //auto it = search(ts::tag_preorder{}, bin, [](auto v) {return v == std::make_pair(15, 20) || v == std::make_pair(12, 15); });
    //std::vector<tree2::value_type> res(begin(it), end(it));
    //std::vector<tree2::value_type> truth({ std::make_pair(15, 20), std::make_pair(12, 15) });
    //assert(res == truth);
}

int main()
{
    std::cout << "INTERVAL TREE IMPLEMENTATION TESTS" << std::endl;
    check_insert();
    check_traversing();
    check_search_iterator_invariants();
    check_search_geeks_example();
    check_search_wiki_example();
    
    generic_search();
}
