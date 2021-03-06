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
using tree = ts::tree<std::pair< T, T>, ts::interval_augment, ts::capability_traverse, ts::capability_insert_interval, ts::capability_remove_interval, ts::capability_search_interval>;

struct compile_time_check_warning {
    struct remove_no_supress {
        using tree = ts::tree<std::pair< int, int>, ts::interval_augment, ts::capability_remove>;
        static_assert(!decltype(tree().warn_)::condition_t::value, "capability_remove check is missing amongst warnings for interval tree. Check condition_t of warning for interval tree.");
    };
    struct insert_no_supress {
        using tree = ts::tree<std::pair< int, int>, ts::interval_augment, ts::capability_insert>;
        static_assert(!decltype(tree().warn_)::condition_t::value, "capability_insert check is missing amongst warnings for interval tree. Check condition_t of warning for interval tree.");
    };
    struct all_supress {
        using tree = ts::tree<std::pair< int, int>, ts::interval_augment, ts::capability_insert, ts::capability_remove, ts::capability_supress_warning_interval>;
        static_assert(decltype(tree().warn_)::condition_t::value, "capability_supress_warning_redblack check is missing amongst warnings for interval tree. Check condition_t of warning for interval tree.");
    };
};

struct compile_time_check_insert_constraints {

    struct mock_augment {

        template <typename T, typename>
        struct mock {
            using value_type = T;
        };

        using tree = ts::tree<std::pair<int, int>, mock, ts::capability_insert_interval>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(insert(T(), typename T::value_type{})) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "Interval tree insert allows augmentation with types other than interval_augment. check enable_interval_insert_t.");
    };

    struct rotation_disabled {

        using tree = ts::tree<std::pair<int, int>, ts::interval_augment, ts::capability_insert_interval, ts::capability_rotate>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(insert(T(), typename T::value_type{})) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "Interval tree insert allows rotation capability to be specified. check enable_interval_insert_t.");
    };
};

struct compile_time_check_remove_constraints {

    struct mock_augment {

        template <typename T, typename>
        struct mock {
            using value_type = T;
        };

        using tree = ts::tree<std::pair<int, int>, mock, ts::capability_remove_interval>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(remove(T(), typename T::value_type{})) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "Interval tree remove allows augmentation with types other than interval_augment. check enable_interval_remove_t.");
    };

    struct rotation_disabled {

        using tree = ts::tree<std::pair<int, int>, ts::interval_augment, ts::capability_remove_interval, ts::capability_rotate>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(remove(T(), typename T::value_type{})) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "Interval tree remove allows rotation capability to be specified. check enable_interval_remove_t.");
    };
};

struct compile_time_check_generic_search {
    template< typename, typename = std::void_t<> >
    struct is_generic_search_absent : std::true_type { };

    template< typename T >
    struct is_generic_search_absent < T,
        std::void_t<decltype(search(ts::tag_preorder{}, T(), std::function<bool(typename T::value_type)>())) >>
        : std::false_type { };

    using tree = ts::tree<std::pair< int, int>, ts::interval_augment, ts::capability_traverse, ts::capability_insert_interval, ts::capability_search_interval, ts::capability_search>;

    static_assert(is_generic_search_absent<tree>::value, "generic search for interval tree must be disabled. interval tree has its own search.");
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

    using tree = ts::tree<std::pair< int, int>, ts::interval_augment, ts::capability_traverse>;

    static_assert(is_iterator_const<tree, ts::tag_preorder>::value, "preorder search iterator should return const reference.");
    static_assert(is_iterator_const<tree, ts::tag_inorder>::value, "inorder search iterator should return const reference.");
    static_assert(is_iterator_const<tree, ts::tag_postorder>::value, "postorder search iterator should return const reference.");
};

// Thorough insertion tests are performed in the rb tree testing project; However max updates checking is essential
void check_insert() {

    std::cout << "checking lrvalue/list/iterator insertion ...";
    tree<int> bin;
    insert(bin, std::make_pair(1, 10)); // rvalue

    //                         (1, 10):10
    assert(ts::aux::access(bin)->value_ == std::make_pair(1, 10));
    assert(ts::aux::access(bin)->max_ == 10);

    assert(optimal(bin));

    insert(bin, { std::make_pair(-7, 3), std::make_pair(-10, 20) }); // init list

    //                         (-7, 3):20
    //          (-10, 20):20               (1, 10):10
    assert(ts::aux::access(bin)->value_ == std::make_pair(-7, 3));
    assert(ts::aux::access(bin)->max_ == 20);

    assert(ts::aux::access(bin)->left_->value_ == std::make_pair(-10, 20));
    assert(ts::aux::access(bin)->left_->max_ == 20);
    assert(ts::aux::access(bin)->right_->value_ == std::make_pair(1, 10));
    assert(ts::aux::access(bin)->right_->max_ == 10);

    assert(optimal(bin));

    auto vv = std::make_pair(17, 35);
    insert(bin, vv); // lvalue

    //                         (-7, 3):35
    //          (-10, 20):20               (1, 10):35
    //                                          (17, 35):35
    assert(ts::aux::access(bin)->value_ == std::make_pair(-7, 3));
    assert(ts::aux::access(bin)->max_ == 35);

    assert(ts::aux::access(bin)->left_->value_ == std::make_pair(-10, 20));
    assert(ts::aux::access(bin)->left_->max_ == 20);
    assert(ts::aux::access(bin)->right_->value_ == std::make_pair(1, 10));
    assert(ts::aux::access(bin)->right_->max_ == 35);

    assert(ts::aux::access(bin)->right_->right_->value_ == std::make_pair(17, 35));
    assert(ts::aux::access(bin)->right_->right_->max_ == 35);

    assert(optimal(bin));

    std::vector<std::pair<int, int>> b{std::make_pair(40, 100), std::make_pair(50, 120), std::make_pair(60, 68) };
    insert(bin, std::begin(b), std::end(b));
    //                         (-7, 3):120
    //          (-10, 20):20               (17, 35):120
    //                           (1, 10):10         (50, 120):120
    //                                        (40, 100):100 (60, 68):68
    assert(ts::aux::access(bin)->value_ == std::make_pair(-7, 3));
    assert(ts::aux::access(bin)->max_ == 120);

    assert(ts::aux::access(bin)->left_->value_ == std::make_pair(-10, 20));
    assert(ts::aux::access(bin)->left_->max_ == 20);
    assert(ts::aux::access(bin)->right_->value_ == std::make_pair(17, 35));
    assert(ts::aux::access(bin)->right_->max_ == 120);

    assert(ts::aux::access(bin)->right_->left_->value_ == std::make_pair(1, 10));
    assert(ts::aux::access(bin)->right_->left_->max_ == 10);
    assert(ts::aux::access(bin)->right_->right_->value_ == std::make_pair(50, 120));
    assert(ts::aux::access(bin)->right_->right_->max_ == 120);

    assert(ts::aux::access(bin)->right_->right_->left_->value_ == std::make_pair(40, 100));
    assert(ts::aux::access(bin)->right_->right_->left_->max_ == 100);
    assert(ts::aux::access(bin)->right_->right_->right_->value_ == std::make_pair(60, 68));
    assert(ts::aux::access(bin)->right_->right_->right_->max_ == 68);

    assert(optimal(bin));

    std::cout << "ok" << std::endl;
}

// Thorough removal tests are performed in the rb tree testing project; However max updates checking is essential
void check_remove() {

    std::cout << "checking lrvalue/list/iterator removal ...";
    tree<int> bin;
    //                         (-7, 3):300
    //          (-10, 20):300               (1, 10):35
    //  (-50, 300):300                                (17, 35):35
    insert(bin, {std::make_pair(1, 10), std::make_pair(-7, 3), std::make_pair(-10, 20), std::make_pair(17, 35), std::make_pair(-50, 300) });
    
    //                         (-7, 3):35
    //          (-10, 20):20               (1, 10):35
    //                                             (17, 35):35
    remove(bin, std::make_pair(-50, 300)); // rvalue
    assert(ts::aux::access(bin)->value_ == std::make_pair(-7, 3));
    assert(ts::aux::access(bin)->max_ == 35);

    assert(ts::aux::access(bin)->left_->value_ == std::make_pair(-10, 20));
    assert(ts::aux::access(bin)->left_->max_ == 20);
    assert(ts::aux::access(bin)->right_->value_ == std::make_pair(1, 10));
    assert(ts::aux::access(bin)->right_->max_ == 35);

    assert(ts::aux::access(bin)->right_->right_->value_ == std::make_pair(17, 35));
    assert(ts::aux::access(bin)->right_->right_->max_ == 35);

    assert(optimal(bin));

    remove(bin, { std::make_pair(1, 10), std::make_pair(17, 35) }); // init list

    //                         (-7, 3):20
    //          (-10, 20):20               
    //                                     
    assert(ts::aux::access(bin)->value_ == std::make_pair(-7, 3));
    assert(ts::aux::access(bin)->max_ == 20);

    assert(ts::aux::access(bin)->left_->value_ == std::make_pair(-10, 20));
    assert(ts::aux::access(bin)->left_->max_ == 20);

    assert(optimal(bin));

    auto a = std::make_pair(-7, 3); // lvalue
    remove(bin, a);
    
    //                         (-10, 20):20
    assert(ts::aux::access(bin)->value_ == std::make_pair(-10, 20));
    assert(ts::aux::access(bin)->max_ == 20);

    assert(optimal(bin));

    std::vector<std::pair<int, int>> b{ std::make_pair(-10, 20) };
    remove(bin, std::begin(b), std::end(b));
    assert(ts::aux::access(bin) == nullptr);

    assert(optimal(bin));

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

void check_insert_string() {

    std::cout << "checking with strings ...";
    tree<std::string> bin;
    insert(bin
        , { std::make_pair(std::string("abc"), std::string("klm"))
          , std::make_pair(std::string("abcfedf"), std::string("lkmng"))
          , std::make_pair(std::string("sdffgh33"), std::string("0938490"))
        });
    // preorder
    {
        auto it = search(ts::tag_preorder{}, bin, std::make_pair(std::string("abc"), std::string("xyz")));
        std::vector<tree<std::string>::value_type> res(begin(it), end(it));
        std::vector<tree<std::string>::value_type> truth{ std::make_pair(std::string("abcfedf"), std::string("lkmng"))
                                                        , std::make_pair(std::string("abc"), std::string("klm")) };
        assert(res == truth);
    }
    std::cout << "ok" << std::endl;
}

int main()
{
    std::cout << "INTERVAL TREE IMPLEMENTATION TESTS" << std::endl;
    check_insert();
    check_remove();
    check_traversing();
    check_search_iterator_invariants();
    check_search_geeks_example();
    check_search_wiki_example();
    check_insert_string();
}
