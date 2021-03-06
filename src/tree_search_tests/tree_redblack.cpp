/* Additional material
*/

#include "tree_search/tree_redblack.hpp"
#include "tree_search/capability_insert_redblack.hpp"
#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_remove_redblack.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_search.hpp"
#include "tree_search/tree.hpp"
#include "tree_search/compile_time_debug.hpp"

#include <cassert>
#include <iostream>
#include <vector>

namespace ts = tree_search;

template <typename T>
using tree = ts::tree<T, ts::redblack_augment, ts::capability_traverse, ts::capability_insert_redblack, ts::capability_remove_redblack, ts::capability_search>;

struct compile_time_check_warning {
    struct remove_no_supress {
        using tree = ts::tree<int, ts::redblack_augment, ts::capability_remove>;
        static_assert(!decltype(tree().warn_)::condition_t::value, "capability_remove check is missing amongst warnings for RB-tree. Check condition_t of warning for RB.");
    };
    struct insert_no_supress {
        using tree = ts::tree<int, ts::redblack_augment, ts::capability_insert>;
        static_assert(!decltype(tree().warn_)::condition_t::value, "capability_insert check is missing amongst warnings for RB-tree. Check condition_t of warning for RB.");
    };
    struct all_supress {
        using tree = ts::tree<int, ts::redblack_augment, ts::capability_insert, ts::capability_remove, ts::capability_supress_warning_redblack>;
        static_assert(decltype(tree().warn_)::condition_t::value, "capability_supress_warning_redblack check is missing amongst warnings for RB-tree. Check condition_t of warning for RB.");
    };
};

struct compile_time_check_insert_constraints {

    struct mock_augment {
      
        template <typename T, typename>
        struct mock {
            using value_type = T;
        };

        using tree = ts::tree<int, mock, ts::capability_insert_redblack>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(insert(T(), 0)) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "RB-tree insert allows augmentation with types other than redblack_augment. check enable_redblack_insert_t.");
    };

    struct rotation_disabled {

        using tree = ts::tree<int, ts::redblack_augment, ts::capability_insert_redblack, ts::capability_rotate>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(insert(T(), 0)) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "RB-tree insert allows rotation capability to be specified. check enable_redblack_insert_t.");
    };
};

struct compile_time_check_remove_constraints {

    struct mock_augment {

        template <typename T, typename>
        struct mock {
            using value_type = T;
        };

        using tree = ts::tree<int, mock, ts::capability_remove_redblack>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(remove(T(), 0)) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "RB-tree remove allows augmentation with types other than redblack_augment. check enable_redblack_remove_t.");
    };

    struct rotation_disabled {

        using tree = ts::tree<int, ts::redblack_augment, ts::capability_remove_redblack, ts::capability_rotate>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(remove(T(), 0)) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "RB-tree remove allows rotation capability to be specified. check enable_redblack_remove_t.");
    };
};

void check_internal_rotation() {

    std::cout << "checking internal rotation ...";
    // left
    {
        //          12b
        //    10b         16r       
        //            15b     33b   
        //          14r     18r 44r
        tree<int> bin;
        insert(bin, 10);
        insert(bin, 12);
        insert(bin, 15);
        insert(bin, 18);
        insert(bin, 16);
        insert(bin, 14);
        insert(bin, 44);
        insert(bin, 33);
 
        assert(ts::aux::access(bin)->value_ == 12);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);

        assert(ts::aux::access(bin)->left_->value_ == 10);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);

        assert(ts::aux::access(bin)->right_->value_ == 16);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_red);

        assert(ts::aux::access(bin)->right_->left_->value_ == 15);
        assert(ts::aux::access(bin)->right_->left_->color_ == ts::rb_black);

        assert(ts::aux::access(bin)->right_->right_->value_ == 33);
        assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_black);

        assert(ts::aux::access(bin)->right_->left_->left_->value_ == 14);
        assert(ts::aux::access(bin)->right_->left_->left_->color_ == ts::rb_red);

        assert(ts::aux::access(bin)->right_->right_->left_->value_ == 18);
        assert(ts::aux::access(bin)->right_->right_->left_->color_ == ts::rb_red);

        assert(ts::aux::access(bin)->right_->right_->right_->value_ == 44);
        assert(ts::aux::access(bin)->right_->right_->right_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    // right
    {
        //               8b
        //         4r          10b
        //    -13b    5b         
        // -24r  2r      6r 
        tree<int> bin;
        insert(bin, 10); 
        insert(bin, 8);
        insert(bin, 5);
        insert(bin, 2);
        insert(bin, 4);
        insert(bin, 6);
        insert(bin, -24);
        insert(bin, -13);

        assert(ts::aux::access(bin)->value_ == 8);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);

        assert(ts::aux::access(bin)->right_->value_ == 10);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);

        assert(ts::aux::access(bin)->left_->value_ == 4);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_red);

        assert(ts::aux::access(bin)->left_->left_->value_ == -13);
        assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_black);

        assert(ts::aux::access(bin)->left_->right_->value_ == 5);
        assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_black);

        assert(ts::aux::access(bin)->left_->left_->left_->value_ == -24);
        assert(ts::aux::access(bin)->left_->left_->left_->color_ == ts::rb_red);

        assert(ts::aux::access(bin)->left_->left_->right_->value_ == 2);
        assert(ts::aux::access(bin)->left_->left_->right_->color_ == ts::rb_red);

        assert(ts::aux::access(bin)->left_->right_->right_->value_ == 6);
        assert(ts::aux::access(bin)->left_->right_->right_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    std::cout << "ok" << std::endl;
}

void check_insert_lrvalue() {

    std::cout << "checking lrvalue/list/iterator insertion (including existing elements) ...";
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

        assert(optimal(bin));
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

        assert(optimal(bin));
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

        assert(optimal(bin));
    }
    std::vector<int> b{50, 60, 70};
    insert(bin, std::begin(b), std::end(b));
    {
        //                         10
        //              7                       18
        //         3         8            15          26
        //                             11    16    22    60
        //                                             50  70
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 15, 11, 16, 26, 22, 60, 50, 70 });
        assert(res == truth);

        assert(optimal(bin));
    }
    {
        struct A {
            int a_;
            int b_;
            A(int a, int b) : a_(a), b_(b) {}
            A(A&& a) {
                std::swap(a.a_, a_);
                std::swap(a.b_, b_);
            }
            A(const A& a) = default;
            bool operator <(const A& v) const { return a_ < v.a_; }
            bool operator ==(const A& v) const { return a_ == v.a_; }
        };
        tree<A> bin;
        insert(bin, {A(1, 10), A(2, 20)});
        insert(bin, A{ 1, 100 });

        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<A>::value_type> res(begin(it), end(it));
        std::vector<tree<A>::value_type> truth({ A(1, 10), A(2, 20) });
        assert(res == truth);
    }
    std::cout << "ok" << std::endl;
}

void check_insert_or_assign_lrvalue() {

    std::cout << "checking lrvalue/list/iterator insert_or_assign (including existing elements) ...";
    tree<int> bin;
    insert_or_assign(bin, { 7, 3, 18, 10, 22, 8, 11, 26 }); // init list
    {
        //                          7
        //              3                       18
        //                                10          22
        //                             8     11          26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 7, 3, 18, 10, 8, 11, 22, 26 });
        assert(res == truth);

        assert(optimal(bin));
    }
    insert_or_assign(bin, 15); // rvalue
    {
        //                         10
        //              7                       18
        //         3         8            11          22
        //                                   15          26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 11, 15, 22, 26 });
        assert(res == truth);

        assert(optimal(bin));
    }
    int a = 16;
    insert_or_assign(bin, a); // lvalue
    {
        //                         10
        //              7                       18
        //         3         8            15          22
        //                             11    16          26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 15, 11, 16, 22, 26 });
        assert(res == truth);

        assert(optimal(bin));
    }
    std::vector<int> b{ 50, 60, 70 };
    insert_or_assign(bin, std::begin(b), std::end(b));
    {
        //                         10
        //              7                       18
        //         3         8            15          26
        //                             11    16    22    60
        //                                             50  70
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 15, 11, 16, 26, 22, 60, 50, 70 });
        assert(res == truth);

        assert(optimal(bin));
    }
    {
        struct A {
            int a_;
            int b_;
            A(int a, int b) : a_(a), b_(b) {}
            A(A&& a) {
                std::swap(a.a_, a_);
                std::swap(a.b_, b_);
            }
            A(const A& a) = default;
            A& operator =(const A&) = default;
            bool operator <(const A& v) const { return a_ < v.a_; }
            bool operator ==(const A& v) const { return a_ == v.a_; }
        };
        tree<A> bin;
        insert_or_assign(bin, { A(1, 10), A(2, 20) });
        insert_or_assign(bin, A{ 1, 100 });

        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<A>::value_type> res(begin(it), end(it));
        std::vector<tree<A>::value_type> truth({ A(1, 100), A(2, 20) });
        assert(res == truth);
    }
    std::cout << "ok" << std::endl;
}

void check_remove_lrvalue() {

    std::cout << "checking lrvalue/list/iterator removal ...";
    tree<int> bin;
    insert(bin, { 7, 3, 18, 10, 22, 8, 11, 26 }); // init list
    //                          7
    //              3                       18
    //                                10          22
    //                             8     11          26
    remove(bin, 10); // rvalue
    {
        //                         7
        //              3                       18
        //                                11          22
        //                             8                 26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 7, 3, 18, 11, 8, 22, 26 });
        assert(res == truth);

        assert(optimal(bin));
    }
    int a = 18;
    remove(bin, a); // lvalue
    {
        //                         7
        //              3                       22
        //                                11          26
        //                             8                
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 7, 3, 22, 11, 8, 26 });
        assert(res == truth);

        assert(optimal(bin));
    }
    std::vector<int> b{ 7, 22, 11 };
    remove(bin, std::begin(b), std::end(b));
    {
        //                         8
        //              3                       26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 8, 3, 26 });
        assert(res == truth);

        assert(optimal(bin));
    }
    std::cout << "ok" << std::endl;

}

void check_remove_case2() {
    /* DIRECT CHANGES OF INTERNAL FLAGS EMPLOYED HERE ARE FOR TESTING PURPOSES ONLY AND OUGHT NOT BE USED ANYWHERE OUTSIDE THE TESTS.
       such a measure is merely to simplify fitting a tree accoring to the particular case being tested.
    */
    std::cout << "checking invariants after removal: case 2 ...";
    // right
    {
        tree<int> bin;
        //                      10b
        //             -10b             40b
        //         -20b     -5b     20b     60r
        //                                50b 80b
        insert(bin, { 10, -10, 40, -20, -5, 20, 60, 50, 80 });
        ts::aux::access(bin)->color_ = ts::rb_black; // 10
        ts::aux::access(bin)->left_->color_ = ts::rb_black; //-10
        ts::aux::access(bin)->right_->color_ = ts::rb_black; // 40
        ts::aux::access(bin)->left_->left_->color_ = ts::rb_black; // -20
        ts::aux::access(bin)->left_->right_->color_ = ts::rb_black; // -5
        ts::aux::access(bin)->right_->left_->color_ = ts::rb_black; // 20
        ts::aux::access(bin)->right_->right_->color_ = ts::rb_red; // 60
        ts::aux::access(bin)->right_->right_->left_->color_ = ts::rb_black; // 50
        ts::aux::access(bin)->right_->right_->right_->color_ = ts::rb_black; // 80

        remove(bin, 10);
        //                       20b
        assert(ts::aux::access(bin)->value_ == 20);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //             -10b                60b   
        assert(ts::aux::access(bin)->left_->value_ == -10);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->value_ == 60);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
        //      -20b        -5b      40b          80b
        assert(ts::aux::access(bin)->left_->left_->value_ == -20);
        assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->left_->right_->value_ == -5);
        assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->left_->value_ == 40);
        assert(ts::aux::access(bin)->right_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->right_->value_ == 80);
        assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_black);
        //                              50r
        assert(ts::aux::access(bin)->right_->left_->right_->value_ == 50);
        assert(ts::aux::access(bin)->right_->left_->right_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    // left
    {
        tree<int> bin;
        //                      10b
        //             -10b             40b
        //       -20r         -5b   20b     60b
        //   -30b    -17b
        insert(bin, { 10, -10, 40, -20, -5, 20, 60, -30, -17 });
        ts::aux::access(bin)->color_ = ts::rb_black; // 10
        ts::aux::access(bin)->left_->color_ = ts::rb_black; //-10
        ts::aux::access(bin)->right_->color_ = ts::rb_black; // 40
        ts::aux::access(bin)->left_->left_->color_ = ts::rb_red; // -20
        ts::aux::access(bin)->left_->right_->color_ = ts::rb_black; // -5
        ts::aux::access(bin)->right_->left_->color_ = ts::rb_black; // 20
        ts::aux::access(bin)->right_->right_->color_ = ts::rb_black; // 60
        ts::aux::access(bin)->left_->left_->left_->color_ = ts::rb_black; // -30
        ts::aux::access(bin)->left_->left_->right_->color_ = ts::rb_black; // -17

        remove(bin, -5);
        //                       10b
        assert(ts::aux::access(bin)->value_ == 10);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //             -20b                40b   
        assert(ts::aux::access(bin)->left_->value_ == -20);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->value_ == 40);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
        //      -30b        -10b      20b          60b
        assert(ts::aux::access(bin)->left_->left_->value_ == -30);
        assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->left_->right_->value_ == -10);
        assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->left_->value_ == 20);
        assert(ts::aux::access(bin)->right_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->right_->value_ == 60);
        assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_black);
        //              -17r
        assert(ts::aux::access(bin)->left_->right_->left_->value_ == -17);
        assert(ts::aux::access(bin)->left_->right_->left_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    std::cout << "ok" << std::endl;
}

void check_remove_case3() {
    /* DIRECT CHANGES OF INTERNAL FLAGS EMPLOYED HERE ARE FOR TESTING PURPOSES ONLY AND OUGHT NOT BE USED ANYWHERE OUTSIDE THE TESTS.
       such a measure is merely to simplify fitting a tree accoring to the particular case being tested.
    */
    std::cout << "checking invariants after removal: case 3 ...";
    // right simple
    {
        tree<int> bin;
        //                      10b
        //             -10b             30b
        insert(bin, { 10, -10, 30 });
        ts::aux::access(bin)->color_ = ts::rb_black;
        ts::aux::access(bin)->left_->color_ = ts::rb_black;
        ts::aux::access(bin)->right_->color_ = ts::rb_black;

        remove(bin, -10);
        //                       10b
        assert(ts::aux::access(bin)->value_ == 10);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //                              30r   
        assert(ts::aux::access(bin)->right_->value_ == 30);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    // left simple
    {
        tree<int> bin;
        //                      10b
        //             -10b             30b
        insert(bin, { 10, -10, 30 });
        ts::aux::access(bin)->color_ = ts::rb_black;
        ts::aux::access(bin)->left_->color_ = ts::rb_black;
        ts::aux::access(bin)->right_->color_ = ts::rb_black;

        remove(bin, 30);
        //                       10b
        assert(ts::aux::access(bin)->value_ == 10);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //                -10r   
        assert(ts::aux::access(bin)->left_->value_ == -10);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    std::cout << "ok" << std::endl;
}

void check_remove_case4() {
    /* DIRECT CHANGES OF INTERNAL FLAGS EMPLOYED HERE ARE FOR TESTING PURPOSES ONLY AND OUGHT NOT BE USED ANYWHERE OUTSIDE THE TESTS.
       such a measure is merely to simplify fitting a tree accoring to the particular case being tested.
    */
    std::cout << "checking invariants after removal: case 4 ...";
    // right
    {
        tree<int> bin;
        //                      10b
        //             -10b             30r
        //                          20b     38b
        insert(bin, { 10, -10, 30, 20, 38 });
        ts::aux::access(bin)->color_ = ts::rb_black;
        ts::aux::access(bin)->left_->color_ = ts::rb_black;
        ts::aux::access(bin)->right_->color_ = ts::rb_red;
        ts::aux::access(bin)->right_->left_->color_ = ts::rb_black;
        ts::aux::access(bin)->right_->right_->color_ = ts::rb_black;

        remove(bin, 20);
        //                       10b
        assert(ts::aux::access(bin)->value_ == 10);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //             -10b             30b   
        assert(ts::aux::access(bin)->left_->value_ == -10);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->value_ == 30);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
        //                                  38r   
        assert(ts::aux::access(bin)->right_->right_->value_ == 38);
        assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    // left
    {
        tree<int> bin;
        //                      10b
        //             -10r             30b
        //       -20b        -5b
        insert(bin, { 10, -10, 30, -20, -5 });
        ts::aux::access(bin)->color_ = ts::rb_black;
        ts::aux::access(bin)->right_->color_ = ts::rb_black;
        ts::aux::access(bin)->left_->color_ = ts::rb_red;
        ts::aux::access(bin)->left_->right_->color_ = ts::rb_black;
        ts::aux::access(bin)->left_->left_->color_ = ts::rb_black;

        remove(bin, -5);
        //                       10b
        assert(ts::aux::access(bin)->value_ == 10);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //             -10b             30b   
        assert(ts::aux::access(bin)->left_->value_ == -10);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->value_ == 30);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
        //       -20r
        assert(ts::aux::access(bin)->left_->left_->value_ == -20);
        assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    std::cout << "ok" << std::endl;
}

void check_remove_case5() {
    /* DIRECT CHANGES OF INTERNAL FLAGS EMPLOYED HERE ARE FOR TESTING PURPOSES ONLY AND OUGHT NOT BE USED ANYWHERE OUTSIDE THE TESTS.
       such a measure is merely to simplify fitting a tree accoring to the particular case being tested.
    */
    std::cout << "checking invariants after removal: case 3->5->6 ...";
    // right
    {
        tree<int> bin;
        //                      10b
        //             -30b             50b
        //         -40b     -20b    30r     70b
        //                        15b 40b
        insert(bin, { 10, -30, 50, -40, -20, 30, 70, 15, 40 });
        ts::aux::access(bin)->color_ = ts::rb_black; // 10
        ts::aux::access(bin)->left_->color_ = ts::rb_black; //-30
        ts::aux::access(bin)->right_->color_ = ts::rb_black; // 50
        ts::aux::access(bin)->left_->left_->color_ = ts::rb_black; // -40
        ts::aux::access(bin)->left_->right_->color_ = ts::rb_black; // -20
        ts::aux::access(bin)->right_->left_->color_ = ts::rb_red; // 30
        ts::aux::access(bin)->right_->right_->color_ = ts::rb_black; // 70
        ts::aux::access(bin)->right_->left_->left_->color_ = ts::rb_black; // 15
        ts::aux::access(bin)->right_->left_->right_->color_ = ts::rb_black; // 40

        remove(bin, -40);
        //                       30b
        assert(ts::aux::access(bin)->value_ == 30);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //             10b                 50b   
        assert(ts::aux::access(bin)->left_->value_ == 10);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->value_ == 50);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
        //      -30b        15b      40b          70b
        assert(ts::aux::access(bin)->left_->left_->value_ == -30);
        assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->left_->right_->value_ == 15);
        assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->left_->value_ == 40);
        assert(ts::aux::access(bin)->right_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->right_->value_ == 70);
        assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_black);
        //          -20r
        assert(ts::aux::access(bin)->left_->left_->right_->value_ == -20);
        assert(ts::aux::access(bin)->left_->left_->right_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    // left
    {
        tree<int> bin;
        //                      10b
        //             -30b             50b
        //         -40b     -20r    30b     70b
        //               -25b -10b
        insert(bin, { 10, -30, 50, -40, -20, 30, 70, -25, -10 });
        ts::aux::access(bin)->color_ = ts::rb_black; // 10
        ts::aux::access(bin)->left_->color_ = ts::rb_black; //-30
        ts::aux::access(bin)->right_->color_ = ts::rb_black; // 50
        ts::aux::access(bin)->left_->left_->color_ = ts::rb_black; // -40
        ts::aux::access(bin)->left_->right_->color_ = ts::rb_red; // -20
        ts::aux::access(bin)->right_->left_->color_ = ts::rb_black; // 30
        ts::aux::access(bin)->right_->right_->color_ = ts::rb_black; // 70
        ts::aux::access(bin)->left_->right_->left_->color_ = ts::rb_black; // -25
        ts::aux::access(bin)->left_->right_->right_->color_ = ts::rb_black; // -10

        remove(bin, 70);
        //                       -20b
        assert(ts::aux::access(bin)->value_ == -20);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //            -30b                 10b   
        assert(ts::aux::access(bin)->left_->value_ == -30);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->value_ == 10);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
        //      -40b       -25b      -10b         50b
        assert(ts::aux::access(bin)->left_->left_->value_ == -40);
        assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->left_->right_->value_ == -25);
        assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->left_->value_ == -10);
        assert(ts::aux::access(bin)->right_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->right_->value_ == 50);
        assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_black);
        //                                      30r
        assert(ts::aux::access(bin)->right_->right_->left_->value_ == 30);
        assert(ts::aux::access(bin)->right_->right_->left_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    std::cout << "ok" << std::endl;
}

void check_remove_case6() {
    /* DIRECT CHANGES OF INTERNAL FLAGS EMPLOYED HERE ARE FOR TESTING PURPOSES ONLY AND OUGHT NOT BE USED ANYWHERE OUTSIDE THE TESTS.
       such a measure is merely to simplify fitting a tree accoring to the particular case being tested.
    */
    std::cout << "checking invariants after removal: case 6 ...";
    // right
    {
        tree<int> bin;
        //                      10b
        //             -10b             30b
        //                          25r     40r
        insert(bin, { 10, -10, 30, 25, 40 });
        ts::aux::access(bin)->color_ = ts::rb_black;
        ts::aux::access(bin)->left_->color_ = ts::rb_black;
        ts::aux::access(bin)->right_->color_ = ts::rb_black;
        ts::aux::access(bin)->right_->left_->color_ = ts::rb_red;
        ts::aux::access(bin)->right_->right_->color_ = ts::rb_red;

        remove(bin, -10);
        //                       30b
        assert(ts::aux::access(bin)->value_ == 30);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //              10b             40b   
        assert(ts::aux::access(bin)->left_->value_ == 10);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->value_ == 40);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
        //                   25r   
        assert(ts::aux::access(bin)->left_->right_->value_ == 25);
        assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    // left
    {
        tree<int> bin;
        //                      10
        //             -10b             30b
        //       -40r         -5
        insert(bin, { 10, -10, 30, -40, -5 });
        //ts::aux::access(bin)->color_ = ts::rb_black; color doesn't matter
        ts::aux::access(bin)->left_->color_ = ts::rb_black;
        ts::aux::access(bin)->right_->color_ = ts::rb_black;
        ts::aux::access(bin)->left_->left_->color_ = ts::rb_red;
        // ts::aux::access(bin)->left_->right_->color_ = ts::rb_red;  // color doesn't matter

        remove(bin, 30);
        //                       -10
        assert(ts::aux::access(bin)->value_ == -10); // color doesn't matter
        //              -40b             10b   
        assert(ts::aux::access(bin)->left_->value_ == -40);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->value_ == 10);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
        //                           -5   
        assert(ts::aux::access(bin)->right_->left_->value_ == -5); // color doesn't matter

        assert(optimal(bin));
    }
    std::cout << "ok" << std::endl;
}

void check_remove_simple_cases() {
    std::cout << "checking invariants after removal: trivial red-node cases ...";
    //                      10b
    //               5b             30r
    //          -5r     7r      20b     38b
    //                                35r
    {
        tree<int> bin;
        insert(bin, {10, 30, 5, -5, 7, 20, 38, 35});
        remove(bin, 30); // essentially it will delete 35r
        //                  10b
        assert(ts::aux::access(bin)->value_ == 10);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //          5r                 35r
        assert(ts::aux::access(bin)->left_->value_ == 5);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->value_ == 35);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_red);
        //      -5b    7b           20b  38b
        assert(ts::aux::access(bin)->left_->left_->value_ == -5);
        assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_red);
        assert(ts::aux::access(bin)->left_->right_->value_ == 7);
        assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_red);
        assert(ts::aux::access(bin)->right_->left_->value_ == 20);
        assert(ts::aux::access(bin)->right_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->right_->value_ == 38);
        assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_black);

        assert(optimal(bin));
    }
    //                      30b
    //              10r             38r
    //           5b    20b      32b     41b
    //        -5r 7r              35r
    {
        tree<int> bin;
        insert(bin, { 10, 30, 5, -5, 7, 20, 38, 32, 41, 35 });
        remove(bin, 30); // essentially it will delete 35r
        //                              32
        assert(ts::aux::access(bin)->value_ == 32);
        assert(ts::aux::access(bin)->color_ == ts::rb_black);
        //                      10              38             
        assert(ts::aux::access(bin)->left_->value_ == 10);
        assert(ts::aux::access(bin)->left_->color_ == ts::rb_red);
        assert(ts::aux::access(bin)->right_->value_ == 38);
        assert(ts::aux::access(bin)->right_->color_ == ts::rb_red);
        //                  5       20      35      41               
        assert(ts::aux::access(bin)->left_->left_->value_ == 5);
        assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->left_->right_->value_ == 20);
        assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->left_->value_ == 35);
        assert(ts::aux::access(bin)->right_->left_->color_ == ts::rb_black);
        assert(ts::aux::access(bin)->right_->right_->value_ == 41);
        assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_black);
        //              -5    7
        assert(ts::aux::access(bin)->left_->left_->left_->value_ == -5);
        assert(ts::aux::access(bin)->left_->left_->left_->color_ == ts::rb_red);
        assert(ts::aux::access(bin)->left_->left_->right_->value_ == 7);
        assert(ts::aux::access(bin)->left_->left_->right_->color_ == ts::rb_red);

        assert(optimal(bin));
    }
    std::cout << "ok" << std::endl;
}

void check_insert_rb_invariants() {

    std::cout << "checking insert invariants ...";
    tree<int> bin;
    insert(bin, { 11, 5, 15, 4, 7, 16, 8 });
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->color_ == ts::rb_black);
    assert(ts::aux::access(bin)->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->color_ == ts::rb_red);
    assert(ts::aux::access(bin)->left_->left_->value_ == 4);
    assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_black);
    assert(ts::aux::access(bin)->left_->right_->value_ == 7);
    assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_black);
    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 8);
    assert(ts::aux::access(bin)->left_->right_->right_->color_ == ts::rb_red);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);
    assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_red);

    assert(optimal(bin));

    insert(bin, 9);
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->color_ == ts::rb_black);
    assert(ts::aux::access(bin)->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->color_ == ts::rb_red);
    assert(ts::aux::access(bin)->left_->left_->value_ == 4);
    assert(ts::aux::access(bin)->left_->left_->color_ == ts::rb_black);
    assert(ts::aux::access(bin)->left_->right_->value_ == 8);
    assert(ts::aux::access(bin)->left_->right_->color_ == ts::rb_black);
    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 9);
    assert(ts::aux::access(bin)->left_->right_->right_->color_ == ts::rb_red);
    assert(ts::aux::access(bin)->left_->right_->left_->value_ == 7);
    assert(ts::aux::access(bin)->left_->right_->left_->color_ == ts::rb_red);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->color_ == ts::rb_black);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);
    assert(ts::aux::access(bin)->right_->right_->color_ == ts::rb_red);

    assert(optimal(bin));
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

    check_internal_rotation();
    check_insert_rb_invariants();
    check_insert_lrvalue();
    check_insert_or_assign_lrvalue();
    check_remove_lrvalue();
    check_remove_simple_cases();
    check_remove_case2();
    check_remove_case3();
    check_remove_case4();
    check_remove_case5();
    check_remove_case6();
    check_search();
}
