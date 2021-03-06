/* Additional material
*/

#include "tree_search/tree_avl.hpp"
#include "tree_search/tree.hpp"
#include "tree_search/capability_search.hpp"
#include "tree_search/capability_traverse.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace ts = tree_search;

template <typename T>
using tree = ts::tree<T, ts::avl_augment, ts::capability_insert_avl, ts::capability_remove_avl, ts::capability_search, ts::capability_traverse>;

struct compile_time_check_warning {
    struct remove_no_supress {
        using tree = ts::tree<int, ts::avl_augment, ts::capability_remove>;
        static_assert(!decltype(tree().warn_)::condition_t::value, "capability_remove check is missing amongst warnings for AVL-tree. Check condition_t of warning for AVL.");
    };
    struct insert_no_supress {
        using tree = ts::tree<int, ts::avl_augment, ts::capability_insert>;
        static_assert(!decltype(tree().warn_)::condition_t::value, "capability_insert check is missing amongst warnings for AVL-tree. Check condition_t of warning for AVL.");
    };
    struct all_supress {
        using tree = ts::tree<int, ts::avl_augment, ts::capability_insert, ts::capability_remove, ts::capability_supress_warning_avl>;
        static_assert(decltype(tree().warn_)::condition_t::value, "capability_supress_warning_avl check is missing amongst warnings for AVL-tree. Check condition_t of warning for AVL.");
    };
};

struct compile_time_check_insert_constraints {

    struct mock_augment {

        template <typename T, typename>
        struct mock {
            using value_type = T;
        };

        using tree = ts::tree<int, mock, ts::capability_insert_avl>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(insert(T(), 0)) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "AVL-tree insert allows augmentation with types other than avl_augment. check enable_avl_insert_t.");
    };

    struct rotation_disabled {

        using tree = ts::tree<int, ts::avl_augment, ts::capability_insert_avl, ts::capability_rotate>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(insert(T(), 0)) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "AVL-tree insert allows rotation capability to be specified. check enable_avl_insert_t.");
    };
};

struct compile_time_check_remove_constraints {

    struct mock_augment {

        template <typename T, typename>
        struct mock {
            using value_type = T;
        };

        using tree = ts::tree<int, mock, ts::capability_remove_avl>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(remove(T(), 0)) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "AVL-tree remove allows augmentation with types other than avl_augment. check enable_avl_remove_t.");
    };

    struct rotation_disabled {

        using tree = ts::tree<int, ts::avl_augment, ts::capability_remove_avl, ts::capability_rotate>;

        template< typename, typename = std::void_t<> >
        struct is_checked : std::true_type { };

        template< typename T >
        struct is_checked < T,
            std::void_t<decltype(remove(T(), 0)) >
        > : std::false_type { };

        static_assert(is_checked<tree>::value, "AVL-tree remove allows rotation capability to be specified. check enable_avl_remove_t.");
    };
};

void check_insert_lrvalue() {

    std::cout << "checking lrvalue/list/iterator insertion (including existing elements) ...";
    tree<int> bin;
    insert(bin, { 7, 3, 18, 10, 22, 8, 11, 26 }); // init list
    {
        //                          10
        //              7                       18
        //         3         8            11          22
        //                                               26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 11, 22, 26 });
        assert(res == truth);

        assert(balanced(bin));
    }
    insert(bin, 15); // rvalue
    {
        //                          10
        //              7                       18
        //         3         8            11          22
        //                                   15          26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 11, 15, 22, 26 });
        assert(res == truth);

        assert(balanced(bin));
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

        assert(balanced(bin));
    }
    std::vector<int> b{ 50, 60, 70 };
    insert(bin, std::begin(b), std::end(b));
    {
        //                         18
        //              10                       26
        //         7         15            22          60
        //       3   8    11    16                  50    70
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 18, 10, 7, 3, 8, 15, 11, 16, 26, 22, 60, 50, 70 });
        assert(res == truth);

        assert(balanced(bin));
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
        insert(bin, { A(1, 10), A(2, 20) });
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
        //                          10
        //              7                       18
        //         3         8            11          22
        //                                               26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 11, 22, 26 });
        assert(res == truth);

        assert(balanced(bin));
    }
    insert_or_assign(bin, 15); // rvalue
    {
        //                          10
        //              7                       18
        //         3         8            11          22
        //                                   15          26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 10, 7, 3, 8, 18, 11, 15, 22, 26 });
        assert(res == truth);

        assert(balanced(bin));
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

        assert(balanced(bin));
    }
    std::vector<int> b{ 50, 60, 70 };
    insert_or_assign(bin, std::begin(b), std::end(b));
    {
        //                         18
        //              10                       26
        //         7         15            22          60
        //       3   8    11    16                  50    70
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 18, 10, 7, 3, 8, 15, 11, 16, 26, 22, 60, 50, 70 });
        assert(res == truth);

        assert(balanced(bin));
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
            A& operator=(const A&) = default;
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
    //                          10
    //              7                       18
    //         3         8            11          22
    //                                               26
    remove(bin, 10); // rvalue
    {
        //                         11
        //              7                       22
        //          3       8             18          26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 11, 7, 3, 8, 22, 18, 26 });
        assert(res == truth);

        assert(balanced(bin));
    }
    int a = 18;
    remove(bin, a); // lvalue
    {
        //                         11
        //              7                       22
        //          3       8                         26
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<tree<int>::value_type> res(begin(it), end(it));
        std::vector<tree<int>::value_type> truth({ 11, 7, 3, 8, 22, 26 });
        assert(res == truth);

        assert(balanced(bin));
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

        assert(balanced(bin));
    }
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

void check_size() {

    std::cout << "checking AVL size ...";
    tree<int> bin;
    insert(bin, 11); // rvalue
    insert(bin, { 5, 15, 4, 7, 16 }); // initializer_list
    int a = 8;
    insert(bin, a); // lvalue
    assert(size(bin) == 7);
    std::cout << "ok" << std::endl;
}

void check_right_right_insert() {

    std::cout << "checking AVL invariant for right-right insert ...";
    tree<int> bin;
    insert(bin, { 11, 5, 15, 4, 7, 16, 8 });
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->left_->value_ == 4);
    assert(ts::aux::access(bin)->left_->right_->value_ == 7);
    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 8);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);

    assert(balanced(bin));

    insert(bin, 9);
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->left_->value_ == 4);
    assert(ts::aux::access(bin)->left_->right_->value_ == 8);
    assert(ts::aux::access(bin)->left_->right_->left_->value_ == 7);
    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 9);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);

    assert(balanced(bin));

    std::cout << "ok" << std::endl;
}

void check_right_left_insert() {

    std::cout << "checking AVL invariant for right-left insert ...";
    tree<int> bin;
    insert(bin, { 11, 5, 15, 4, 7, 16, 9 });
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->left_->value_ == 4);
    assert(ts::aux::access(bin)->left_->right_->value_ == 7);
    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 9);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);

    assert(balanced(bin));

    insert(bin, 8);
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->left_->value_ == 4);
    assert(ts::aux::access(bin)->left_->right_->value_ == 8);
    assert(ts::aux::access(bin)->left_->right_->left_->value_ == 7);
    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 9);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);

    assert(balanced(bin));

    std::cout << "ok" << std::endl;
}

void check_left_left_insert() {

    std::cout << "checking AVL invariant for left-left insert ...";
    tree<int> bin;
    insert(bin, { 11, 7, 15, 5, 9, 16, 3 });
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->left_->value_ == 7);
    assert(ts::aux::access(bin)->left_->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->left_->left_->value_ == 3);
    assert(ts::aux::access(bin)->left_->right_->value_ == 9);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);

    assert(balanced(bin));

    insert(bin, 1);
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->left_->value_ == 7);
    assert(ts::aux::access(bin)->left_->left_->value_ == 3);
    assert(ts::aux::access(bin)->left_->left_->left_->value_ == 1);
    assert(ts::aux::access(bin)->left_->left_->right_->value_ == 5);
    assert(ts::aux::access(bin)->left_->right_->value_ == 9);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);

    assert(balanced(bin));

    std::cout << "ok" << std::endl;
}

void check_left_right_insert() {

    std::cout << "checking AVL invariant for left-right insert ...";
    tree<int> bin;
    insert(bin, { 11, 7, 15, 5, 9, 16, 3 });
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->left_->value_ == 7);
    assert(ts::aux::access(bin)->left_->left_->value_ == 5);
    assert(ts::aux::access(bin)->left_->left_->left_->value_ == 3);
    assert(ts::aux::access(bin)->left_->right_->value_ == 9);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);

    assert(balanced(bin));

    insert(bin, 4);
    assert(ts::aux::access(bin)->value_ == 11);
    assert(ts::aux::access(bin)->left_->value_ == 7);
    assert(ts::aux::access(bin)->left_->left_->value_ == 4);
    assert(ts::aux::access(bin)->left_->left_->left_->value_ == 3);
    assert(ts::aux::access(bin)->left_->left_->right_->value_ == 5);
    assert(ts::aux::access(bin)->left_->right_->value_ == 9);
    assert(ts::aux::access(bin)->right_->value_ == 15);
    assert(ts::aux::access(bin)->right_->right_->value_ == 16);

    assert(balanced(bin));


    std::cout << "ok" << std::endl;
}

void check_remove() {

    std::cout << "checking AVL invariant for remove ...";
    tree<int> bin;
    insert(bin, { 44, 17, 78, 32, 50, 88, 48, 62 });
    //                  44
    //      17                      78
    //           32            50        88
    //                      48    62
    assert(ts::aux::access(bin)->value_ == 44);
    assert(ts::aux::access(bin)->left_->value_ == 17);
    assert(ts::aux::access(bin)->right_->value_ == 78);
    assert(ts::aux::access(bin)->left_->right_->value_ == 32);
    assert(ts::aux::access(bin)->right_->left_->value_ == 50);
    assert(ts::aux::access(bin)->right_->right_->value_ == 88);
    assert(ts::aux::access(bin)->right_->left_->left_->value_ == 48);
    assert(ts::aux::access(bin)->right_->left_->right_->value_ == 62);

    assert(balanced(bin));

    remove(bin, 32);
    //                  50
    //      44                      78
    // 17        48            62        88
    assert(ts::aux::access(bin)->value_ == 50);
    assert(ts::aux::access(bin)->left_->value_ == 44);
    assert(ts::aux::access(bin)->right_->value_ == 78);
    assert(ts::aux::access(bin)->left_->left_->value_ == 17);
    assert(ts::aux::access(bin)->left_->right_->value_ == 48);
    assert(ts::aux::access(bin)->right_->left_->value_ == 62);
    assert(ts::aux::access(bin)->right_->right_->value_ == 88);

    assert(balanced(bin));


    std::cout << "ok" << std::endl;
}

void check_remove_case1() {

    std::cout << "checking AVL invariant for case 1 remove ...";
    tree<int> bin;
    insert(bin, { 2, 1, 4, 3, 5 });
    //                  2
    //       1                      4
    //                         3         5
    assert(ts::aux::access(bin)->value_ == 2);
    assert(ts::aux::access(bin)->left_->value_ == 1);
    assert(ts::aux::access(bin)->right_->value_ == 4);
    assert(ts::aux::access(bin)->right_->left_->value_ == 3);
    assert(ts::aux::access(bin)->right_->right_->value_ == 5);

    assert(balanced(bin));

    remove(bin, 1);
    //                  4
    //      2                      5
    //           3              
    assert(ts::aux::access(bin)->value_ == 4);
    assert(ts::aux::access(bin)->left_->value_ == 2);
    assert(ts::aux::access(bin)->right_->value_ == 5);
    assert(ts::aux::access(bin)->left_->right_->value_ == 3);

    assert(balanced(bin));


    std::cout << "ok" << std::endl;
}

void check_remove_case2() {

    std::cout << "checking AVL invariant for case 2 remove ...";
    tree<int> bin;
    insert(bin, { 6, 2, 9, 1, 4, 8, 12, 3, 5, 7, 10, 15, 20 });
    //                  6
    //       2                      9
    //  1         4            8         12
    //          3   5        7        10    15
    //                                         20
    assert(ts::aux::access(bin)->value_ == 6);
    assert(ts::aux::access(bin)->left_->value_ == 2);
    assert(ts::aux::access(bin)->right_->value_ == 9);

    assert(ts::aux::access(bin)->left_->left_->value_ == 1);
    assert(ts::aux::access(bin)->left_ ->right_->value_ == 4);
    assert(ts::aux::access(bin)->right_->left_->value_ == 8);
    assert(ts::aux::access(bin)->right_->right_->value_ == 12);

    assert(ts::aux::access(bin)->left_->right_->left_->value_ == 3);
    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 5);
    assert(ts::aux::access(bin)->right_->left_->left_->value_ == 7);
    assert(ts::aux::access(bin)->right_->right_->left_->value_ == 10);
    assert(ts::aux::access(bin)->right_->right_->right_->value_ == 15);

    assert(ts::aux::access(bin)->right_->right_->right_->right_->value_ == 20);

    assert(balanced(bin));

    remove(bin, 1);
    //                  6
    //       4                      9
    //  2         5            8         12
    //     3                 7        10    15
    //                                         20
    assert(ts::aux::access(bin)->value_ == 6);
    assert(ts::aux::access(bin)->left_->value_ == 4);
    assert(ts::aux::access(bin)->right_->value_ == 9);

    assert(ts::aux::access(bin)->left_->left_->value_ == 2);
    assert(ts::aux::access(bin)->left_->right_->value_ == 5);
    assert(ts::aux::access(bin)->right_->left_->value_ == 8);
    assert(ts::aux::access(bin)->right_->right_->value_ == 12);

    assert(ts::aux::access(bin)->left_->left_->right_->value_ == 3);
    assert(ts::aux::access(bin)->left_->right_->value_ == 5);
    assert(ts::aux::access(bin)->right_->left_->left_->value_ == 7);
    assert(ts::aux::access(bin)->right_->right_->left_->value_ == 10);
    assert(ts::aux::access(bin)->right_->right_->right_->value_ == 15);

    assert(ts::aux::access(bin)->right_->right_->right_->right_->value_ == 20);

    assert(balanced(bin));

    std::cout << "ok" << std::endl;
}

void check_remove_case3() {

    std::cout << "checking AVL invariant for case 3 remove ...";
    tree<int> bin;
    insert(bin, { 5, 2, 8, 1, 3, 7, 15, 4, 6, 11, 20, 25 });
    //                  5
    //       2                      8
    //  1         3            7         15
    //              4        6        11    20
    //                                         25
    assert(ts::aux::access(bin)->value_ == 5);
    assert(ts::aux::access(bin)->left_->value_ == 2);
    assert(ts::aux::access(bin)->right_->value_ == 8);

    assert(ts::aux::access(bin)->left_->left_->value_ == 1);
    assert(ts::aux::access(bin)->left_->right_->value_ == 3);
    assert(ts::aux::access(bin)->right_->left_->value_ == 7);
    assert(ts::aux::access(bin)->right_->right_->value_ == 15);

    assert(ts::aux::access(bin)->left_->right_->right_->value_ == 4);
    assert(ts::aux::access(bin)->right_->left_->left_->value_ == 6);
    assert(ts::aux::access(bin)->right_->right_->left_->value_ == 11);
    assert(ts::aux::access(bin)->right_->right_->right_->value_ == 20);

    assert(ts::aux::access(bin)->right_->right_->right_->right_->value_ == 25);

    assert(balanced(bin));

    remove(bin, 1);
    //                  8
    //       5                      15
    //  3         7            11        20
    //2   4     6                           25
    assert(ts::aux::access(bin)->value_ == 8);
    assert(ts::aux::access(bin)->left_->value_ == 5);
    assert(ts::aux::access(bin)->right_->value_ == 15);

    assert(ts::aux::access(bin)->left_->left_->value_ == 3);
    assert(ts::aux::access(bin)->left_->right_->value_ == 7);
    assert(ts::aux::access(bin)->right_->left_->value_ == 11);
    assert(ts::aux::access(bin)->right_->right_->value_ == 20);

    assert(ts::aux::access(bin)->left_->left_->left_->value_ == 2);
    assert(ts::aux::access(bin)->left_->left_->right_->value_ == 4);
    assert(ts::aux::access(bin)->left_->right_->left_->value_ == 6);
    assert(ts::aux::access(bin)->right_->right_->right_->value_ == 25);

    assert(balanced(bin));

    std::cout << "ok" << std::endl;
}

int main()
{
    std::cout << "AVL TREE IMPLEMENTATION TESTS" << std::endl;

    check_size();
    check_insert_lrvalue();
    check_insert_or_assign_lrvalue();
    check_remove_lrvalue();
    check_right_right_insert();
    check_right_left_insert();
    check_left_left_insert();
    check_left_right_insert();
    check_search();
    check_remove();
    check_remove_case1();
    check_remove_case2();
    check_remove_case3();
}
