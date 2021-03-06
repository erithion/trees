/* Additional material
*/

#include "tree_search/tree.hpp"
#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_rotate.hpp"
#include "tree_search/tree_avl.hpp"
#include "tree_search/tree_redblack.hpp"
#include "tree_search/capability_insert_redblack.hpp"
#include "tree_search/tree_interval.hpp"
#include "tree_search/capability_search_interval.hpp"
#include <cassert>
#include <iostream>

namespace ts = tree_search;

template <typename T>
using tree_simple = ts::tree<T, ts::empty_augment ,ts::capability_traverse, ts::capability_insert, ts::capability_rotate>;

template <typename T>
using tree_avl = ts::tree<T, ts::avl_augment, ts::capability_insert_avl>;

template <typename T>
using tree_redblack = ts::tree<T, ts::redblack_augment, ts::capability_traverse, ts::capability_insert_redblack>;

template <typename T>
using tree_interval = ts::tree<std::pair< T, T>, ts::interval_augment, ts::capability_traverse, ts::capability_insert_interval, ts::capability_search_interval>;

struct compile_time_check_no_search {

    template< typename, typename = std::void_t<> >
    struct is_checked : std::true_type { };

    template< typename T >
    struct is_checked < T,
        std::void_t<decltype(search(ts::tag_inorder{}, T(), 0)) >
    > : std::false_type { };

    static_assert(is_checked<tree_simple<int>>::value, "overloading resolution: search must not be found.");
    static_assert(is_checked<tree_avl<int>>::value, "overloading resolution: search must not be found.");
    static_assert(is_checked<tree_redblack<int>>::value, "overloading resolution: search must not be found.");
};

// TODO: add tests to check remove overload resolution
int main()
{
    std::cout << "OVERLOADING RESOLUTION TESTS" << std::endl;
    // insert
    {
        std::cout << "inserting into various trees ...";
        tree_simple<int> bin_simple;
        tree_avl<int> bin_avl;
        tree_redblack<int> bin_redblack;
        tree_interval<int> bin_interval;
        // rvalues
        insert(bin_simple, 2);
        insert(bin_avl, 2);
        insert(bin_redblack, 2);
        insert(bin_interval, std::make_pair(20, 36));
        // initializer_list
        insert(bin_simple, { 7, 4, 6, 3, 8, 1 });
        insert(bin_avl, { 7, 4, 6, 3, 8, 1 });
        insert(bin_redblack, { 7, 4, 6, 3, 8, 1 });
        insert(bin_interval, 
            { std::make_pair(3, 41)
              , std::make_pair(29, 99)
              , std::make_pair(0, 1) });
        // lvalues
        int a = 5;
        auto p = std::make_pair(10, 15);
        insert(bin_simple, a);
        insert(bin_avl, a);
        insert(bin_redblack, a);
        insert(bin_interval, p);

        assert(ts::aux::access(bin_simple)->value_ == 2);
        assert(ts::aux::access(bin_simple)->left_->value_ == 1);
        assert(ts::aux::access(bin_simple)->right_->value_ == 7);
        assert(ts::aux::access(bin_simple)->right_->left_->value_ == 4);
        assert(ts::aux::access(bin_simple)->right_->right_->value_ == 8);
        assert(ts::aux::access(bin_simple)->right_->left_->left_->value_ == 3);
        assert(ts::aux::access(bin_simple)->right_->left_->right_->value_ == 6);
        assert(ts::aux::access(bin_simple)->right_->left_->right_->left_->value_ == 5);

        assert(ts::aux::access(bin_avl)->value_ == 4);
        assert(ts::aux::access(bin_avl)->left_->value_ == 2);
        assert(ts::aux::access(bin_avl)->right_->value_ == 7);
        assert(ts::aux::access(bin_avl)->left_->left_->value_ == 1);
        assert(ts::aux::access(bin_avl)->left_->right_->value_ == 3);
        assert(ts::aux::access(bin_avl)->right_->left_->value_ == 6);
        assert(ts::aux::access(bin_avl)->right_->right_->value_ == 8);
        assert(ts::aux::access(bin_avl)->right_->left_->left_->value_ == 5);

        assert(ts::aux::access(bin_redblack)->value_ == 4);
        assert(ts::aux::access(bin_redblack)->left_->value_ == 2);
        assert(ts::aux::access(bin_redblack)->right_->value_ == 7);
        assert(ts::aux::access(bin_redblack)->left_->left_->value_ == 1);
        assert(ts::aux::access(bin_redblack)->left_->right_->value_ == 3);
        assert(ts::aux::access(bin_redblack)->right_->left_->value_ == 6);
        assert(ts::aux::access(bin_redblack)->right_->right_->value_ == 8);
        assert(ts::aux::access(bin_redblack)->right_->left_->left_->value_ == 5);

        assert(ts::aux::access(bin_interval)->value_ == std::make_pair(20, 36));
        assert(ts::aux::access(bin_interval)->max_ == 99);
        assert(ts::aux::access(bin_interval)->left_->value_ == std::make_pair(3, 41));
        assert(ts::aux::access(bin_interval)->left_->max_ == 41);
        assert(ts::aux::access(bin_interval)->right_->value_ == std::make_pair(29, 99));
        assert(ts::aux::access(bin_interval)->right_->max_ == 99);
        assert(ts::aux::access(bin_interval)->left_->left_->value_ == std::make_pair(0, 1));
        assert(ts::aux::access(bin_interval)->left_->left_->max_ == 1);
        assert(ts::aux::access(bin_interval)->left_->right_->value_ == std::make_pair(10, 15));
        assert(ts::aux::access(bin_interval)->left_->right_->max_ == 15);

        for (auto i : search(ts::tag_inorder{}, bin_interval, std::make_pair(-8, 7)));

        std::cout << "ok" << std::endl;
    }
}
