/* Additional material
*/

#include "tree_search/tree.hpp"
#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_rotate.hpp"
#include "tree_search/capability_search.hpp"
#include "tree_search/capability_remove.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <map>

namespace ts = tree_search;

template <typename T>
using tree = ts::tree<T, ts::empty_augment ,ts::capability_traverse, ts::capability_insert, ts::capability_rotate, ts::capability_search, ts::capability_remove>;

void check_insert_lrvalue() {

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

    std::cout << "checking lrvalue/list/iterator insertion (including existing elements) ...";
    tree<A> bin;
    A a{ 1, 10 };
    insert(bin, a); // lvalue
    insert(bin, A{ 2, 20 }); // rvalue
    A tmp{ 3, 30 };
    insert(bin, std::move(tmp)); // rvalue manual-made
    std::vector<A> b{ A{1, 100}, A{5, 50}, A{6, 60} };
    insert(bin, std::begin(b), std::end(b));
    assert(!(tmp == A(3, 30))); // value is moved
    assert(a == A(1, 10)); // value is copied
    assert(ts::aux::access(bin)->value_ == A(1, 10));
    assert(ts::aux::access(bin)->right_->value_ == A(2, 20));
    assert(ts::aux::access(bin)->right_->right_->value_ == A(3, 30));
    assert(ts::aux::access(bin)->right_->right_->right_->value_ == A(5, 50));
    assert(ts::aux::access(bin)->right_->right_->right_->right_->value_ == A(6, 60));

    std::cout << "ok" << std::endl;
}

void check_insert_or_assign_lrvalue() {

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

    std::cout << "checking lrvalue/list/iterator insertion_or_assign (including existing elements) ...";
    tree<A> bin;
    A a{ 1, 10 };
    insert_or_assign(bin, a); // lvalue
    insert_or_assign(bin, A{ 2, 20 }); // rvalue
    A tmp{ 3, 30 };
    insert_or_assign(bin, std::move(tmp)); // rvalue manual-made
    std::vector<A> b{ A{1, 100}, A{5, 50}, A{6, 60} };
    insert_or_assign(bin, std::begin(b), std::end(b));
    assert(!(tmp == A(3, 30))); // value is moved
    assert(a == A(1, 10)); // value is copied
    assert(ts::aux::access(bin)->value_ == A(1, 100));
    assert(ts::aux::access(bin)->right_->value_ == A(2, 20));
    assert(ts::aux::access(bin)->right_->right_->value_ == A(3, 30));
    assert(ts::aux::access(bin)->right_->right_->right_->value_ == A(5, 50));
    assert(ts::aux::access(bin)->right_->right_->right_->right_->value_ == A(6, 60));

    std::cout << "ok" << std::endl;
}

void check_size() {
    std::cout << "checking size ...";
    tree<int> bin;
    insert(bin, { 5, 6, 1, 3, 8, 10, 7, 2 });
    assert(size(bin) == 8);
    std::cout << "ok" << std::endl;
}

void check_traversing_preorder() {
    std::cout << "checking preorder traversing ...";
    // type checks
    {
        tree<char> bin;
        insert(bin, { 'F', 'B' });
        auto it = traverse(ts::tag_preorder{}, bin);

        static_assert (std::is_same_v<decltype(*it.begin_), const char&>, "traverse iterator should return const reference");
    }
    {
        // range loop
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            std::string res = {};
            for (auto v : traverse(ts::tag_preorder{}, bin)) res.push_back(v);
            assert(res == "FBADCEGIH");
        }
        // begin/end
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            auto it = traverse(ts::tag_preorder{}, bin);
            std::string res(it.begin_, it.end_);
            assert(res == "FBADCEGIH");
        }
        // prefix ++
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            auto it = traverse(ts::tag_preorder{}, bin);
            assert(*it.begin_ == 'F');
            assert(*++it.begin_ == 'B');
            assert(*++it.begin_ == 'A');
            assert(*++it.begin_ == 'D');
            assert(*++it.begin_ == 'C');
            assert(*++it.begin_ == 'E');
            assert(*++it.begin_ == 'G');
            assert(*++it.begin_ == 'I');
            assert(*++it.begin_ == 'H');
            assert(++it.begin_ == it.end_);
        }
        // postfix ++
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            auto it = traverse(ts::tag_preorder{}, bin);
            assert(*it.begin_++ == 'F');
            assert(*it.begin_++ == 'B');
            assert(*it.begin_++ == 'A');
            assert(*it.begin_++ == 'D');
            assert(*it.begin_++ == 'C');
            assert(*it.begin_++ == 'E');
            assert(*it.begin_++ == 'G');
            assert(*it.begin_++ == 'I');
            assert(*it.begin_++ == 'H');
            assert(it.begin_++ == it.end_);
        }
    }
    {
        // range loop
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            std::string res = {};
            for (auto v : traverse(ts::tag_preorder{}, bin)) res.push_back(v);
            assert(res == "HDFEGJIK");
        }
        // begin/end
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_preorder{}, bin);
            std::string res(it.begin_, it.end_);
            assert(res == "HDFEGJIK");
        }
        // skewed tree prefix ++
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_preorder{}, bin);
            assert(*it.begin_ == 'H');
            assert(*++it.begin_ == 'D');
            assert(*++it.begin_ == 'F');
            assert(*++it.begin_ == 'E');
            assert(*++it.begin_ == 'G');
            assert(*++it.begin_ == 'J');
            assert(*++it.begin_ == 'I');
            assert(*++it.begin_ == 'K');
            assert(++it.begin_ == it.end_);
        }
        // skewed tree postfix ++
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_preorder{}, bin);
            assert(*it.begin_++ == 'H');
            assert(*it.begin_++ == 'D');
            assert(*it.begin_++ == 'F');
            assert(*it.begin_++ == 'E');
            assert(*it.begin_++ == 'G');
            assert(*it.begin_++ == 'J');
            assert(*it.begin_++ == 'I');
            assert(*it.begin_++ == 'K');
            assert(it.begin_++ == it.end_);
        }
    }
    std::cout << "ok" << std::endl;
}

void check_traversing_inorder() {
    std::cout << "checking inorder traversing ...";
    // type checks
    {
        tree<char> bin;
        insert(bin, { 'F', 'B' });
        auto it = traverse(ts::tag_inorder{}, bin);

        static_assert (std::is_same_v<decltype(*it.begin_), const char&>, "traverse iterator should return const reference");
    }
    {
        // range loop
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            std::string res = {};
            for (auto v : traverse(ts::tag_inorder{}, bin)) res.push_back(v);
            assert(res == "ABCDEFGHI");
        }
        // begin/end
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            auto it = traverse(ts::tag_inorder{}, bin);
            std::string res(it.begin_, it.end_);
            assert(res == "ABCDEFGHI");
        }
        // prefix ++
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            std::vector<char> res;
            auto it = traverse(ts::tag_inorder{}, bin);
            assert(*it.begin_ == 'A');
            assert(*++it.begin_ == 'B');
            assert(*++it.begin_ == 'C');
            assert(*++it.begin_ == 'D');
            assert(*++it.begin_ == 'E');
            assert(*++it.begin_ == 'F');
            assert(*++it.begin_ == 'G');
            assert(*++it.begin_ == 'H');
            assert(*++it.begin_ == 'I');
            assert(++it.begin_ == it.end_);
        }
        // postfix ++
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            std::vector<char> res;
            auto it = traverse(ts::tag_inorder{}, bin);
            assert(*it.begin_++ == 'A');
            assert(*it.begin_++ == 'B');
            assert(*it.begin_++ == 'C');
            assert(*it.begin_++ == 'D');
            assert(*it.begin_++ == 'E');
            assert(*it.begin_++ == 'F');
            assert(*it.begin_++ == 'G');
            assert(*it.begin_++ == 'H');
            assert(*it.begin_++ == 'I');
            assert(it.begin_++ == it.end_);
        }
    }
    {
        // range loop
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            std::string res = {};
            for (auto v : traverse(ts::tag_inorder{}, bin)) res.push_back(v);
            assert(res == "DEFGHIJK");
        }
        // begin/end
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_inorder{}, bin);
            std::string res(it.begin_, it.end_);
            assert(res == "DEFGHIJK");
        }
        // skewed tree prefix ++
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_inorder{}, bin);
            assert(*it.begin_ == 'D');
            assert(*++it.begin_ == 'E');
            assert(*++it.begin_ == 'F');
            assert(*++it.begin_ == 'G');
            assert(*++it.begin_ == 'H');
            assert(*++it.begin_ == 'I');
            assert(*++it.begin_ == 'J');
            assert(*++it.begin_ == 'K');
            assert(++it.begin_ == it.end_);
        }
        // skewed tree postfix ++
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_inorder{}, bin);
            assert(*it.begin_++ == 'D');
            assert(*it.begin_++ == 'E');
            assert(*it.begin_++ == 'F');
            assert(*it.begin_++ == 'G');
            assert(*it.begin_++ == 'H');
            assert(*it.begin_++ == 'I');
            assert(*it.begin_++ == 'J');
            assert(*it.begin_++ == 'K');
            assert(it.begin_++ == it.end_);
        }
    }
    std::cout << "ok" << std::endl;
}

void check_traversing_postorder() {

    std::cout << "checking postorder traversing ...";
    // type checks
    {
        tree<char> bin;
        insert(bin, { 'F', 'B' });
        auto it = traverse(ts::tag_postorder{}, bin);

        static_assert (std::is_same_v<decltype(*it.begin_), const char&>, "traverse iterator should return const reference");
    }
    {
        // range loop
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            std::string res = {};
            for (auto v : traverse(ts::tag_postorder{}, bin)) res.push_back(v);
            assert(res == "ACEDBHIGF");
        }
        // begin/end
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            auto it = traverse(ts::tag_postorder{}, bin);
            std::string res(it.begin_, it.end_);
            assert(res == "ACEDBHIGF");
        }
        // prefix ++
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            auto it = traverse(ts::tag_postorder{}, bin);
            assert(*it.begin_ == 'A');
            assert(*++it.begin_ == 'C');
            assert(*++it.begin_ == 'E');
            assert(*++it.begin_ == 'D');
            assert(*++it.begin_ == 'B');
            assert(*++it.begin_ == 'H');
            assert(*++it.begin_ == 'I');
            assert(*++it.begin_ == 'G');
            assert(*++it.begin_ == 'F');
            assert(++it.begin_ == it.end_);
        }
        // postfix ++
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            auto it = traverse(ts::tag_postorder{}, bin);
            assert(*it.begin_++ == 'A');
            assert(*it.begin_++ == 'C');
            assert(*it.begin_++ == 'E');
            assert(*it.begin_++ == 'D');
            assert(*it.begin_++ == 'B');
            assert(*it.begin_++ == 'H');
            assert(*it.begin_++ == 'I');
            assert(*it.begin_++ == 'G');
            assert(*it.begin_++ == 'F');
            assert(it.begin_++ == it.end_);
        }
    }
    {
        // range loop
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            std::string res = {};
            for (auto v : traverse(ts::tag_postorder{}, bin)) res.push_back(v);
            assert(res == "EGFDIKJH");
        }
        // begin/end
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_postorder{}, bin);
            std::string res(it.begin_, it.end_);
            assert(res == "EGFDIKJH");
        }
        // skewed tree prefix ++
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_postorder{}, bin);
            assert(*it.begin_ == 'E');
            assert(*++it.begin_ == 'G');
            assert(*++it.begin_ == 'F');
            assert(*++it.begin_ == 'D');
            assert(*++it.begin_ == 'I');
            assert(*++it.begin_ == 'K');
            assert(*++it.begin_ == 'J');
            assert(*++it.begin_ == 'H');
            assert(++it.begin_ == it.end_);
        }
        // skewed tree postfix ++
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_postorder{}, bin);
            assert(*it.begin_++ == 'E');
            assert(*it.begin_++ == 'G');
            assert(*it.begin_++ == 'F');
            assert(*it.begin_++ == 'D');
            assert(*it.begin_++ == 'I');
            assert(*it.begin_++ == 'K');
            assert(*it.begin_++ == 'J');
            assert(*it.begin_++ == 'H');
            assert(it.begin_++ == it.end_);
        }
    }
    std::cout << "ok" << std::endl;
}

void check_rotation_right() {
    std::cout << "checking right rotation ...";
    tree<char> bin;
    insert(bin, { 'D', 'B', 'A', 'C', 'E' });
    rotate(bin, ts::rotate_right_tag{});
    assert(ts::aux::access(bin)->value_ == 'B');
    assert(ts::aux::access(bin)->left_->value_ == 'A');
    assert(ts::aux::access(bin)->right_->value_ == 'D');
    assert(ts::aux::access(bin)->right_->left_->value_ == 'C');
    assert(ts::aux::access(bin)->right_->right_->value_ == 'E');
    std::cout << "ok" << std::endl;
}

void check_rotation_left() {
    std::cout << "checking left rotation ...";
    tree<char> bin;
    insert(bin, { 'B', 'A', 'D', 'C', 'E' });
    rotate(bin, ts::rotate_left_tag{});
    assert(ts::aux::access(bin)->value_ == 'D');
    assert(ts::aux::access(bin)->left_->value_ == 'B');
    assert(ts::aux::access(bin)->left_->left_->value_ == 'A');
    assert(ts::aux::access(bin)->left_->right_->value_ == 'C');
    assert(ts::aux::access(bin)->right_->value_ == 'E');
    std::cout << "ok" << std::endl;
}

void check_perfect() {
    std::cout << "checking perfect ...";
    tree<int> bin;
    insert(bin, { 4, 2, 7, 1, 3, 6, 8, 5 });
    assert(false == perfect(bin));

    tree<int> bin2;
    insert(bin2, { 5, 3, 7, 2, 4, 6, 8 });
    assert(true == perfect(bin2));

    tree<int> bin3;
    insert(bin3, { 7, 3, 8, 2, 6 });
    assert(true == perfect(bin3));

    tree<int> bin4;
    insert(bin4, { 7, 3, 8, 2, 6, 4 });
    assert(false == perfect(bin4));

    tree<int> bin5;
    insert(bin5, { 2, 1, 7, 4, 8, 3, 6, 5 });
    assert(false == perfect(bin5));

    tree<int> bin6;
    insert(bin6, { 5, 2, 8, 1, 4, 7, 9, 3 });
    assert(false == perfect(bin6));

    std::cout << "ok" << std::endl;
}

void check_height() {
    std::cout << "checking height ...";
    tree<int> bin;
    insert(bin, { 5, 7, 3, 6, 8, 2, 4, 1 });
    assert(4 == height(bin));
    // Now must be
    //                              5
    //                  3                       7
    //              2       4               6       8
    //          1
    std::cout << "ok" << std::endl;
}

void check_balance() {
    // not balanced
    {
        std::cout << "checking not balanced ...";
        tree<int> bin;
        insert(bin, { 2, 7, 4, 6, 3, 8, 1, 5 });
        assert(false == balanced(bin));
        std::cout << "ok" << std::endl;
    }
    // is balanced
    {
        std::cout << "checking balanced ...";
        tree<int> bin;
        insert(bin, { 5, 7, 3, 6, 8, 2, 4, 1 });
        // Now must be
        //                              5
        //                  3                       7
        //              2       4               6       8
        //          1
        assert(true == balanced(bin));
        std::cout << "ok" << std::endl;
    }
}

void check_search() {

    std::cout << "checking search ...";
    tree<char> bin;
    insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
    // preorder
    {
        auto it = search(ts::tag_preorder{}, bin, [](int v) {return v == 'F' || v == 'D' || v == 'E'; });
        std::vector<int> res(begin(it), end(it));
        assert(res == std::vector<int>({ 'F', 'D', 'E' }));
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
        assert(res == std::vector<int>({ 'E', 'D', 'F' }));
    }
    std::cout << "ok" << std::endl;
}

void check_remove_lrvalue() {
    std::cout << "checking lrvalue/list/iterator remove ...";

    tree<int> bin;
    //                              5
    //                  3                       7
    //              2       4               6       8
    //          1
    insert(bin, { 5, 7, 3, 6, 8, 2, 4, 1 });
    remove(bin, { 3, 7 }); // initializer_list
    {
        //                              5
        //                  4                       8
        //              2                       6       
        //          1
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<int> res(begin(it), end(it));
        std::vector<int> truth({ 5, 4, 2, 1, 8, 6 });
        assert(res == truth);
    }
    remove(bin, 2); // rvalue
    {
        //                              5
        //                  4                       8
        //              1                       6       
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<int> res(begin(it), end(it));
        std::vector<int> truth({ 5, 4, 1, 8, 6 });
        assert(res == truth);
    }
    int a = 5;
    remove(bin, a); // lvalue
    {
        //                              6
        //                  4                       8
        //              1                              
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<int> res(begin(it), end(it));
        std::vector<int> truth({ 6, 4, 1, 8 });
        assert(res == truth);
    }
    std::vector<int> b{4, 1, 6};
    remove(bin, std::begin(b), std::end(b));
    {
        //                              8
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<int> res(begin(it), end(it));
        std::vector<int> truth({ 8 });
        assert(res == truth);
    }
    std::cout << "ok" << std::endl;
}

void check_remove_geeks() {
    std::cout << "checking remove 2 ...";

    {
        tree<int> bin;
        //              50
        //   40                      70
        //                     60          80
        insert(bin, { 50, 40, 70, 60, 80 });
        remove(bin, { 50 }); // initializer_list
        //              60
        //   40                      70
        //                                 80
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<int> res(begin(it), end(it));
        std::vector<int> truth({ 60, 40, 70, 80 });
        assert(res == truth);
    }
    {
        tree<int> bin;
        //              50
        //   40                      70
        //                                 80
        insert(bin, { 50, 40, 70, 80 });
        remove(bin, { 50 }); // initializer_list
        //              70
        //   40                      80
        auto it = traverse(ts::tag_preorder{}, bin);
        std::vector<int> res(begin(it), end(it));
        std::vector<int> truth({ 70, 40, 80 });
        assert(res == truth);
    }
    std::cout << "ok" << std::endl;
}

int main()
{
    std::map<int, int> m;
    m.insert(std::make_pair(1, 1));
    m.insert(std::make_pair(1, 10));
    std::cout << "DEFAULT TREE IMPLEMENTATION TESTS" << std::endl;
    // const
    {
        tree<char> bin;
        insert(bin, { 'F', 'B' });
    }
    // checking default template calls
    {
        std::cout << "checking default template calls from custom tree ...";
        tree<int> bin;
        insert(bin, 10);
        assert(ts::aux::access(bin)->value_ == 10);
        std::cout << "ok" << std::endl;
    }
    check_size();
    check_insert_lrvalue();
    check_insert_or_assign_lrvalue();
    check_remove_lrvalue();
    check_traversing_preorder();
    check_traversing_inorder();
    check_traversing_postorder();
    check_search();
    check_rotation_right();
    check_rotation_left();
    check_balance();
    check_height();
    check_perfect();
    check_remove_geeks();
}
