/* Additional material
*/

#include "tree_search/tree_balance.hpp"
#include "tree_search/tree.hpp"
#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_rotate.hpp"
#include <cassert>
#include <iostream>

namespace ts = tree_search;

template <typename T>
using tree = ts::tree<T, ts::aux::empty, ts::capability_insert, ts::capability_rotate, ts::capability_balance_manual>;

// Must fail to compile due to absense of rotation capability
//template <typename T>
//using tree = ts::tree<T, ts::aux::empty, ts::capability_insert, ts::capability_balance_manual>;

// must fail to compile: manual balance capability is incompatible with auto-balancing trees, 
// i.e. only with ts::insert_cap && and ts::rotate_cap present
//#include "tree_search/tree_redblack.hpp"
//template <typename T>
//using tree = ts::tree<T, ts::aux::redblack_augment, ts::capability_insert_redblack, ts::capability_balance_manual>;

int main()
{
    std::cout << "BINARY TREE WITH WARREN-STOUT BALANCING IMPLEMENTATION TESTS" << std::endl;
    // checking straighting the tree up
/*    {
        std::cout << "checking transformation to degenerate right list ...";
        tree<int> bin;
        insert(bin, { 5, 6, 1, 3, 8, 10, 7, 2 });
        auto sz = to_right_list(bin);
        assert(bin->value_ == 1);
        assert(bin->right_->value_ == 2);
        assert(bin->right_->right_->value_ == 3);
        assert(bin->right_->right_->right_->value_ == 5);
        assert(bin->right_->right_->right_->right_->value_ == 6);
        assert(bin->right_->right_->right_->right_->right_->value_ == 7);
        assert(bin->right_->right_->right_->right_->right_->right_->value_ == 8);
        assert(bin->right_->right_->right_->right_->right_->right_->right_->value_ == 10);
        assert(bin->right_->right_->right_->right_->right_->right_->right_->right_ == nullptr);
        assert(sz == 8);
        std::cout << "ok" << std::endl;
    }*/
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
        assert(bin.root_->value_ == 5);
        assert(bin.root_->left_->value_ == 3);
        assert(bin.root_->left_->left_->value_ == 2);
        assert(bin.root_->left_->left_->left_->value_ == 1);
        assert(bin.root_->left_->right_->value_ == 4);
        assert(bin.root_->right_->value_ == 7);
        assert(bin.root_->right_->left_->value_ == 6);
        assert(bin.root_->right_->right_->value_ == 8);
        std::cout << "ok" << std::endl;
    }
}
