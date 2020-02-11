/* Additional material
*/

#include "tree_search/tree.hpp"
#include "tree_search/capability_insert.hpp"
#include "tree_search/capability_traverse.hpp"
#include "tree_search/capability_rotate.hpp"
#include <cassert>
#include <iostream>

namespace ts = tree_search;

template <typename T>
using tree = ts::tree<T, ts::aux::empty ,ts::capability_traverse, ts::capability_insert, ts::capability_rotate>;

int main()
{
    std::cout << "DEFAULT TREE IMPLEMENTATION TESTS" << std::endl;
    // checking default template calls
    {
        std::cout << "checking default template calls from custom tree ...";
        tree<int> bin;
        insert(bin, 10);
        assert(bin.root_->value_ == 10);
        std::cout << "ok" << std::endl;
    }
    // checking lvalue/rvalue insert
    {
        struct A {
            A(int a) : a_(a) {
            }
            A(A&& a) {
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
        assert(bin.root_->value_.a_ == 10);
        assert(bin.root_->right_->value_.a_ == 20);
        assert(bin.root_->right_->right_->value_.a_ == 30);
        std::cout << "ok" << std::endl;
    }
    // checking size
    {
        std::cout << "checking size ...";
        tree<int> bin;
        insert(bin, { 5, 6, 1, 3, 8, 10, 7, 2 });
        assert(size(bin) == 8);
        std::cout << "ok" << std::endl;
    }
    // checking preorder traversing
    {
        std::cout << "checking preorder traversing ...";
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            auto it = traverse(ts::tag_preorder{}, bin);

            static_assert (std::is_same_v<decltype(*it.begin), const char&>, "traverse iterator should return const reference");

            assert(*it.begin++ == 'F');
            assert(*it.begin == 'B');
            assert(*++it.begin == 'A');
            assert(*it.begin++ == 'A');
            assert(*it.begin == 'D');
            assert(*++it.begin == 'C');
            assert(*++it.begin == 'E');
            assert(*++it.begin == 'G');
            assert(*++it.begin == 'I');
            assert(*++it.begin == 'H');
            assert(++it.begin == it.end);
        }
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_preorder{}, bin);

            assert(*it.begin++ == 'H');
            assert(*it.begin == 'D');
            assert(*++it.begin == 'F');
            assert(*it.begin++ == 'F');
            assert(*it.begin == 'E');
            assert(*++it.begin == 'G');
            assert(*++it.begin == 'J');
            assert(*++it.begin == 'I');
            assert(*++it.begin == 'K');
            assert(++it.begin == it.end);
        }

        std::cout << "ok" << std::endl;
    }
    // checking postorder traversing
    {
        std::cout << "checking postorder traversing ...";
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            auto it = traverse(ts::tag_postorder{}, bin);

            static_assert (std::is_same_v<decltype(*it.begin), const char&>, "traverse iterator should return const reference");

            assert(*it.begin++ == 'A');
            assert(*it.begin == 'C');
            assert(*++it.begin == 'E');
            assert(*it.begin++ == 'E');
            assert(*it.begin == 'D');
            assert(*++it.begin == 'B');
            assert(*++it.begin == 'H');
            assert(*++it.begin == 'I');
            assert(*++it.begin == 'G');
            assert(*++it.begin == 'F');
            assert(++it.begin == it.end);
        }
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_postorder{}, bin);
            assert(*it.begin == 'E');
            assert(*++it.begin == 'G');
            assert(*it.begin++ == 'G');
            assert(*it.begin == 'F');
            assert(*++it.begin == 'D');
            assert(*++it.begin == 'I');
            assert(*++it.begin == 'K');
            assert(*++it.begin == 'J');
            assert(*++it.begin == 'H');
            assert(++it.begin == it.end);
        }
        std::cout << "ok" << std::endl;
    }
    // checking inorder traversing
    {
        std::cout << "checking inorder traversing ...";
        {
            tree<char> bin;
            insert(bin, { 'F', 'B', 'A', 'D', 'C', 'E', 'G', 'I', 'H' });
            std::vector<char> res;
            auto it = traverse(ts::tag_inorder{}, bin);

            static_assert (std::is_same_v<decltype(*it.begin), const char&>, "traverse iterator should return const reference");

            assert(*it.begin++ == 'A');
            assert(*it.begin == 'B');
            assert(*++it.begin == 'C');
            assert(*it.begin++ == 'C');
            assert(*it.begin == 'D');
            assert(*++it.begin == 'E');
            assert(*++it.begin == 'F');
            assert(*++it.begin == 'G');
            assert(*++it.begin == 'H');
            assert(*++it.begin == 'I');
            assert(++it.begin == it.end);
        }
        {
            tree<char> bin;
            insert(bin, { 'H', 'D', 'F', 'E', 'G', 'J', 'I', 'K' });
            auto it = traverse(ts::tag_inorder{}, bin);
            assert(*it.begin == 'D');
            assert(*++it.begin == 'E');
            assert(*it.begin++ == 'E');
            assert(*it.begin == 'F');
            assert(*++it.begin == 'G');
            assert(*++it.begin == 'H');
            assert(*++it.begin == 'I');
            assert(*++it.begin == 'J');
            assert(*++it.begin == 'K');
            assert(++it.begin == it.end);
        }

        std::cout << "ok" << std::endl;
    }
    // checking right rotation
    {
        std::cout << "checking right rotation ...";
        tree<char> bin;
        insert(bin, { 'D', 'B', 'A', 'C', 'E' });
        rotate(bin, ts::rotate_right_tag{});
        assert(bin.root_->value_ == 'B');
        assert(bin.root_->left_->value_ == 'A');
        assert(bin.root_->right_->value_ == 'D');
        assert(bin.root_->right_->left_->value_ == 'C');
        assert(bin.root_->right_->right_->value_ == 'E');
        std::cout << "ok" << std::endl;
    }
    // checking left rotation
    {
        std::cout << "checking left rotation ...";
        tree<char> bin;
        insert(bin, { 'B', 'A', 'D', 'C', 'E' });
        rotate(bin, ts::rotate_left_tag{});
        assert(bin.root_->value_ == 'D');
        assert(bin.root_->left_->value_ == 'B');
        assert(bin.root_->left_->left_->value_ == 'A');
        assert(bin.root_->left_->right_->value_ == 'C');
        assert(bin.root_->right_->value_ == 'E');
        std::cout << "ok" << std::endl;
    }
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
    // height
    {
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
    // perfect
    {
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
}
