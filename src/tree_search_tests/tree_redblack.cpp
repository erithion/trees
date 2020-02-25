/* Additional material
*/

#include "tree_search/tree_redblack.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_search.hpp"
#include "tree_search/tree.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace ts = tree_search;

template <typename T>
using tree = ts::tree<T, ts::aux::redblack_augment, ts::capability_traverse, ts::capability_insert_redblack, ts::capability_remove_redblack, ts::capability_search>;

// Must fail to compile: capability_insert_redblack works only when redblack_augment type has also been specified
//template <typename T>
//using tree = ts::tree<T, ts::aux::empty, ts::capability_traverse, ts::capability_insert_redblack>;

// Must fail to compile: rotation is forbidden for auto-balancing trees
//template <typename T>
//using tree = ts::tree<T, ts::aux::redblack_augment, ts::capability_rotate, ts::capability_insert_redblack>;
void check_insert() {

    std::cout << "checking insertion ...";
    tree<int> bin;
    insert(bin, { 7, 3, 18, 10, 22, 8, 11, 26 }); // init list
    {
        //                          7
        //              3                       18
        //                                10          22
        //                             8     11          26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({7, 3, 18, 10, 8, 11, 22, 26});
        assert(res == truth);
    }
    insert(bin, 15); // rvalue
    {
        //                         10
        //              7                       18
        //         3         8            11          22
        //                                   15          26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 11, 15, 22, 26 });
        assert(res == truth);
    }
    int a = 16;
    insert(bin, a); // lvalue
    {
        //                         10
        //              7                       18
        //         3         8            15          22
        //                             11    16          26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 15, 11, 16, 22, 26 });
        assert(res == truth);
    }
    std::cout << "ok" << std::endl;

}

void check_remove_rb_invariants() {
    /* DIRECT CHANGES OF LAGS EMPLOYED HERE ARE FOR TESTING PURPOSES ONLY AND OUGHT NOT BE USED ANYWHERE ELSE.
       such a technique allows to fit the trees accoring to all 6 potential cases that could emerge when removing an element.
    */
    std::cout << "checking remove invariants ...";
    // delete r
    //           r
    //       nil   nil
    {
        tree<int> bin;
        insert(bin, {10, 30, 5, -5, 7, 20, 38, 35});
        remove(bin, 30);
        assert(ts::aux::access(bin)->value_ == 10);
        assert(ts::aux::access(bin)->augment_.is_red_ == false);

        assert(ts::aux::access(bin)->left_->value_ == 5);
        assert(ts::aux::access(bin)->left_->augment_.is_red_ == false);
        assert(ts::aux::access(bin)->right_->value_ == 35);
        assert(ts::aux::access(bin)->right_->augment_.is_red_ == true);

        assert(ts::aux::access(bin)->left_->left_->value_ == -5);
        assert(ts::aux::access(bin)->left_->left_->augment_.is_red_ == true);
        assert(ts::aux::access(bin)->left_->right_->value_ == 7);
        assert(ts::aux::access(bin)->left_->right_->augment_.is_red_ == true);


        assert(ts::aux::access(bin)->right_->left_->value_ == 20);
        assert(ts::aux::access(bin)->right_->left_->augment_.is_red_ == false);
        assert(ts::aux::access(bin)->right_->right_->value_ == 38);
        assert(ts::aux::access(bin)->right_->right_->augment_.is_red_ == false);
    }
    // delete r
    //           r
    //       nil   child
    {
        tree<int> bin;
        insert(bin, { 10, 30, 5, -5, 7, 20, 38, 32, 41, 35 });
        remove(bin, 30);
        //                              32
        assert(ts::aux::access(bin)->value_ == 32);
        assert(ts::aux::access(bin)->augment_.is_red_ == false);
        //                      10              38             
        assert(ts::aux::access(bin)->left_->value_ == 10);
        assert(ts::aux::access(bin)->left_->augment_.is_red_ == true);
        assert(ts::aux::access(bin)->right_->value_ == 38);
        assert(ts::aux::access(bin)->right_->augment_.is_red_ == true);
        //                  5       20      35      41               
        assert(ts::aux::access(bin)->left_->left_->value_ == 5);
        assert(ts::aux::access(bin)->left_->left_->augment_.is_red_ == false);
        assert(ts::aux::access(bin)->left_->right_->value_ == 20);
        assert(ts::aux::access(bin)->left_->right_->augment_.is_red_ == false);
        assert(ts::aux::access(bin)->right_->left_->value_ == 35);
        assert(ts::aux::access(bin)->right_->left_->augment_.is_red_ == false);
        assert(ts::aux::access(bin)->right_->right_->value_ == 41);
        assert(ts::aux::access(bin)->right_->right_->augment_.is_red_ == false);
        //              -5    7
        assert(ts::aux::access(bin)->left_->left_->left_->value_ == -5);
        assert(ts::aux::access(bin)->left_->left_->left_->augment_.is_red_ == true);
        assert(ts::aux::access(bin)->left_->left_->right_->value_ == 7);
        assert(ts::aux::access(bin)->left_->left_->right_->augment_.is_red_ == true);
    }
    // case 4
    {
        tree<int> bin;
        //                      10b
        //             -10b             30r
        //                          20b     38b
        insert(bin, { 10, -10, 30, 20, 38 });
        ts::aux::access(bin)->augment_.is_red_ = false;
        ts::aux::access(bin)->left_->augment_.is_red_ = false;
        ts::aux::access(bin)->right_->augment_.is_red_ = true;
        ts::aux::access(bin)->right_->left_->augment_.is_red_ = false;
        ts::aux::access(bin)->right_->right_->augment_.is_red_ = false;

        remove(bin, 20);
        //                       10b
        assert(ts::aux::access(bin)->value_ == 10);
        assert(ts::aux::access(bin)->augment_.is_red_ == false);
        //             -10b             30b   
        assert(ts::aux::access(bin)->left_->value_ == -10);
        assert(ts::aux::access(bin)->left_->augment_.is_red_ == false);
        assert(ts::aux::access(bin)->right_->value_ == 30);
        assert(ts::aux::access(bin)->right_->augment_.is_red_ == false);
        //                                  38r   
        assert(ts::aux::access(bin)->right_->right_->value_ == 38);
        assert(ts::aux::access(bin)->right_->right_->augment_.is_red_ == true);
    }
    std::cout << "ok" << std::endl;

}

void check_insert_rb_invariants() {

    std::cout << "checking insert invariants ...";
    tree<int> bin;
    insert(bin, { 11, 5, 15, 4, 7, 16, 8 });
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->augment_.is_red_ == false);
    assert(ts::aux::access(bin)->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->augment_.is_red_ == true);
    assert(ts::aux::access(bin)->left_->left_->value_ == 4);
    assert(ts::aux::access(bin)->left_->left_->augment_.is_red_ == false);
    assert(ts::aux::access(bin)->left_->right_->value_ == 7);
    assert(ts::aux::access(bin)->left_->right_->augment_.is_red_ == false);
    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 8);
    assert(ts::aux::access(bin)->left_->right_->right_->augment_.is_red_ == true);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->augment_.is_red_ == false);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);
    assert(ts::aux::access(bin)->right_->right_->augment_.is_red_ == true);

    insert(bin, 9);
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->augment_.is_red_ == false);
    assert(ts::aux::access(bin)->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->augment_.is_red_ == true);
    assert(ts::aux::access(bin)->left_->left_->value_ == 4);
    assert(ts::aux::access(bin)->left_->left_->augment_.is_red_ == false);
    assert(ts::aux::access(bin)->left_->right_->value_ == 8);
    assert(ts::aux::access(bin)->left_->right_->augment_.is_red_ == false);
    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 9);
    assert(ts::aux::access(bin)->left_->right_->right_->augment_.is_red_ == true);
    assert(ts::aux::access(bin)->left_->right_->left_->value_ == 7);
    assert(ts::aux::access(bin)->left_->right_->left_->augment_.is_red_ == true);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->augment_.is_red_ == false);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);
    assert(ts::aux::access(bin)->right_->right_->augment_.is_red_ == true);
    std::cout << "ok" << std::endl;
}

void check_search() {

    std::cout << "checking search ...";
    tree<char> bin;
    insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
    // preorder
    {
        auto it = search(ts::tag_preorder{}, bin, [](int v) {return v == 'F' || v == 'D' || v == 'E'; });
        std::vector<int> res(begin(it), end(it));
        assert(res == std::vector<int>({ 'D', 'F', 'E' }));
    }
    // inorder
    {
        auto it = search(ts::tag_inorder{}, bin, [](int v) {return v == 'D' || v == 'E' || v == 'F'; });
        std::vector<int> res(begin(it), end(it));
        assert(res == std::vector<int>({ 'D', 'E', 'F' }));
    }
    // postorder
    {
        auto it = search(ts::tag_postorder{}, bin, [](int v) {return v == 'D' || v == 'E' || v == 'F'; });
        std::vector<int> res(begin(it), end(it));
        assert(res == std::vector<int>({ 'E', 'F', 'D' }));
    }
    std::cout << "ok" << std::endl;
}

int main()
{
    std::cout << "RED-BLACK TREE IMPLEMENTATION TESTS" << std::endl;
    //
    {
        std::cout << "checking red-black tree size ...";
        tree<int> bin;
        insert(bin, 11); // rvalue
        insert(bin, { 5, 15, 4, 7, 16 }); // initializer_list
        int a = 8;
        insert(bin, a); // lvalue
        assert(size(bin) == 7);
        std::cout << "ok" << std::endl;
    }
    // checking inorder traversing
    {
        std::cout << "checking inorder traversing ...";
        tree<int> bin;
        insert(bin, { 11, 5, 15, 4, 7, 16, 8 });
        std::vector<int> res;
        for (auto v : traverse(ts::tag_inorder{}, bin)) res.push_back(v);
        assert(res == std::vector<int>({ 4, 5, 7, 8, 11, 15, 16 }));
        std::cout << "ok" << std::endl;
        // rotate(ts::rotate_right_tag{}, bin); // must emit compile time error for rotate_cap has not been added into the tree
    }

    check_insert_rb_invariants();
    check_insert();
    check_remove_rb_invariants();
    check_search();
}
