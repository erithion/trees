/* Additional material
*/

#include "tree_search/tree.hpp"
#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_rotate.hpp"
#include "tree_search/capability_search.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace ts = tree_search;

template <typename T>
using tree = ts::tree<T, ts::aux::empty ,ts::capability_traverse, ts::capability_insert, ts::capability_rotate, ts::capability_search>;

void check_insert_lrvalue() {
    struct A {
        A(int a) : a_(a) {
        }
        A(A&& a) noexcept {
            std::swap(this->a_, a.a_);
            return;
        }
        A(const A& a) {
            this->a_ = a.a_;
        }

        bool operator <(const A& a) const {
            return this->a_ < a.a_;
        }
        bool operator >(const A& a) const {
            return this->a_ > a.a_;
        }
        int a_;
    };

    std::cout << "checking lvalue/rvalue conformance of insertion ...";
    tree<A> bin;
    A a{ 10 };
    insert(bin, a); // lvalue
    insert(bin, A{ 20 }); // rvalue
    A tmp{ 30 };
    insert(bin, std::move(tmp)); // rvalue manual-made
    assert(tmp.a_ != 30); // value is moved
    assert(a.a_ == 10); // value is copied
    assert(ts::aux::access(bin)->value_.a_ == 10);
    assert(ts::aux::access(bin)->right_->value_.a_ == 20);
    assert(ts::aux::access(bin)->right_->right_->value_.a_ == 30);
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
        assert(true == balanced(bin));
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
    tree<int> bin;
    insert(bin, { 2, 7, 4, 6, 3, 8, 1, 5 });
    auto it = search(ts::tag_preorder{}, bin, [](int v) {return v == 7 || v == 3 || v == 8; });
    std::vector<int> res(begin(it), end(it));
    assert(res == std::vector<int>({7, 3, 8}));
    std::cout << "ok" << std::endl;
}

int main()
{
    std::cout << "DEFAULT TREE IMPLEMENTATION TESTS" << std::endl;
    // const
    {
        tree<char> bin;
        insert(bin, { 'F', 'B' });

        static_assert (std::is_same_v<decltype(ts::aux::access(bin)->value_), const char>, "tree values const invariant is violated");
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
    check_traversing_preorder();
    check_traversing_inorder();
    check_traversing_postorder();
    check_search();
    check_rotation_right();
    check_rotation_left();
    check_balance();
    check_height();
    check_perfect();
}
