/* Additional material
*/

#include "tree_search/tree_redblack.hpp"
#include "tree_search/tree_balance.hpp"
#include "tree_search/tree.hpp"
#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_rotate.hpp"
#include <cassert>
#include <iostream>

namespace ts = tree_search;

template <typename T>
using tree = ts::tree<T, ts::empty_augment, ts::capability_insert, ts::capability_rotate, ts::capability_balance_manual>;

struct compile_time_check_insert_constraints {

    struct no_autobalance {

        using tree1 = ts::tree<int, ts::redblack_augment, ts::capability_insert_redblack, ts::capability_balance_manual>;
        using tree2 = ts::tree<int, ts::redblack_augment, ts::capability_remove_redblack, ts::capability_balance_manual>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(balance(T())) >
        > : std::false_type { };

        static_assert(is_checked<tree1>::value, "balance function is allowed for trees with auto-balance. check enable_manual_balance_t.");
        static_assert(is_checked<tree2>::value, "balance function is allowed for trees with auto-balance. check enable_manual_balance_t.");
    };
};

int main()
{
    std::cout << "BINARY TREE WITH WARREN-STOUT BALANCING IMPLEMENTATION TESTS" << std::endl;
    // checking straighting the tree up
    { // this is internal test
        std::cout << "checking transformation to degenerate right list ...";
        tree<int> bin;
        insert(bin, { 5, 6, 1, 3, 8, 10, 7, 2 });
        auto sz = ts::aux::to_right_list(ts::aux::access(bin));
        assert(ts::aux::access(bin)->value_ == 1);
        assert(ts::aux::access(bin)->right_->value_ == 2);
        assert(ts::aux::access(bin)->right_->right_->value_ == 3);
        assert(ts::aux::access(bin)->right_->right_->right_->value_ == 5);
        assert(ts::aux::access(bin)->right_->right_->right_->right_->value_ == 6);
        assert(ts::aux::access(bin)->right_->right_->right_->right_->right_->value_ == 7);
        assert(ts::aux::access(bin)->right_->right_->right_->right_->right_->right_->value_ == 8);
        assert(ts::aux::access(bin)->right_->right_->right_->right_->right_->right_->right_->value_ == 10);
        assert(ts::aux::access(bin)->right_->right_->right_->right_->right_->right_->right_->right_ == nullptr);
        assert(sz == 8);
        std::cout << "ok" << std::endl;
    }
    // balance
    {
        std::cout << "checking balancing (transforming to a complete tree) ...";
        tree<int> bin;
        insert(bin, 2); // rvalue
        insert(bin, { 7, 4, 6, 3, 8, 1 }); // initializer_list
        int a = 5;
        insert(bin, a); // lvalue
        balance(bin);
        // Now must be
        //                              5
        //                  3                       7
        //              2       4               6       8
        //          1
        assert(ts::aux::access(bin)->value_ == 5);
        assert(ts::aux::access(bin)->left_->value_ == 3);
        assert(ts::aux::access(bin)->left_->left_->value_ == 2);
        assert(ts::aux::access(bin)->left_->left_->left_->value_ == 1);
        assert(ts::aux::access(bin)->left_->right_->value_ == 4);
        assert(ts::aux::access(bin)->right_->value_ == 7);
        assert(ts::aux::access(bin)->right_->left_->value_ == 6);
        assert(ts::aux::access(bin)->right_->right_->value_ == 8);
        std::cout << "ok" << std::endl;
    }
}
