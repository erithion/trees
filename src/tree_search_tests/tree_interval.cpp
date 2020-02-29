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
using tree = ts::tree<std::pair< T, T>, ts::aux::interval_augment, ts::capability_traverse, ts::capability_insert_interval, ts::capability_search_interval>;

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

struct compile_time_check_generic_search {
    template< typename, typename = std::void_t<> >
    struct is_generic_search_absent : std::true_type { };

    template< typename T >
    struct is_generic_search_absent < T,
        std::void_t<decltype(search(ts::tag_preorder{}, T(), std::function<bool(typename T::value_type)>())) >>
        : std::false_type { };

    using tree = ts::tree<std::pair< int, int>, ts::aux::interval_augment, ts::capability_traverse, ts::capability_insert_interval, ts::capability_search_interval, ts::capability_search>;

    static_assert(is_generic_search_absent<tree>::value, "generic search for interval tree must be disabled. interval tree has its own search.");
};

struct compile_time_check_capability_insert_interval {
    
    struct augment_check {

        template <typename T, typename>
        struct mock_augment {
            using value_type = T;
        };

        template< typename, typename = std::void_t<> >
        struct is_augment_check_present : std::true_type { };

        template< typename T >
        struct is_augment_check_present < T,
            std::void_t<decltype(insert(T(), std::make_pair(0, 0)))>
        > : std::false_type { };

        using tree = ts::tree<std::pair< int, int>, mock_augment, ts::capability_insert_interval>;

        static_assert(is_augment_check_present<tree>::value, "capability_insert_interval must be allowed for interval tree (interval_augment) only.");
    };
};

struct compile_time_check_capability_search_interval {

    struct augment_check {

        template <typename T, typename>
        struct mock_augment {
            using value_type = T;
        };

        template< typename, typename = std::void_t<> >
        struct is_augment_check_present : std::true_type { };

        template< typename T >
        struct is_augment_check_present < T,
            std::void_t<decltype(search(ts::tag_inorder{}, T(), std::make_pair(0, 0))) >
        > : std::false_type { };

        using tree = ts::tree<std::pair< int, int>, mock_augment, ts::capability_search_interval>;

        static_assert(is_augment_check_present<tree>::value, "capability_search_interval must be allowed for interval tree (interval_augment) only.");
    };
};

struct compile_time_check_iterator_const {

    template< typename T, typename Tag >
    struct is_iterator_const
        : std::is_same< decltype(*(traverse(Tag{}, T()).begin_))
                      , const typename T::value_type& > { };

    using tree = ts::tree<std::pair< int, int>, ts::aux::interval_augment, ts::capability_traverse>;

    static_assert(is_iterator_const<tree, ts::tag_preorder>::value, "preorder search iterator should return const reference.");
    static_assert(is_iterator_const<tree, ts::tag_inorder>::value, "inorder search iterator should return const reference.");
    static_assert(is_iterator_const<tree, ts::tag_postorder>::value, "postorder search iterator should return const reference.");
};

int main()
{
    insert(tree<int>(), std::make_pair(1,1));
    std::cout << "INTERVAL TREE IMPLEMENTATION TESTS" << std::endl;
    check_insert();
    check_traversing();
    check_search_iterator_invariants();
    check_search_geeks_example();
    check_search_wiki_example();
}
