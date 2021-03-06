/* Additional material
- https://brilliant.org/wiki/red-black-tree/?subtopic=types-and-data-structures&chapter=binary-search-trees  (insert only. deletion is wrong there.)
- MIT Course https://www.youtube.com/watch?v=O3hI9FdxFOM
- Visualisation for testing http://www.cs.armstrong.edu/liang/animation/web/RBTree.html
*/
#pragma once

#include "tree_redblack.hpp"
#include "capability_rotate.hpp"
#include "capability_insert.hpp"
#include "type_cti.hpp"

#include <memory> // std::unique_ptr

namespace tree_search {

    namespace aux {

        // Compliance within aux namespace is not checked in general. 
        // Just capability_insert_redblack is used on and off as a guard to prevent accidental overloading resolution calls
        template <typename Node>
        inline bool is_left_red(const std::unique_ptr<Node>& tree) {
            return tree && tree->left_ && tree->left_->color_ == rb_red;
        }

        template <typename Node>
        inline bool is_right_red(const std::unique_ptr<Node>& tree) {
            return tree && tree->right_ && tree->right_->color_ == rb_red;
        }

        template <typename Node>
        inline bool any_red_child(const std::unique_ptr<Node>& tree) {
            return is_left_red(tree) || is_right_red(tree);
        }

        template <typename Node>
        inline bool any_red_grandchild(const std::unique_ptr<Node>& tree) {
            if (!tree) return false;
            return is_left_red(tree->left_) || is_right_red(tree->left_)
                || is_left_red(tree->right_) || is_right_red(tree->right_);
        }

        // called from insert
        template <typename Node>
        void fixup(std::unique_ptr<Node>& tree, capability_insert_redblack) {
            if (tree && tree->color_ == rb_black) {
                if (is_left_red(tree) && is_right_red(tree) && any_red_grandchild(tree)) { // Case 1 (left & right) -> recolor
                    tree->color_ = rb_red;
                    tree->left_->color_ = rb_black;
                    tree->right_->color_ = rb_black;
                }
                else if (is_left_red(tree) && any_red_child(tree->left_)) {
                    if (is_right_red(tree->left_)) // case 2 - straightening zig-zag
                        rotate(tree->left_, rotate_left_tag{});
                    rotate(tree, rotate_right_tag{}); // case 3
                    //
                    tree->color_ = rb_black;
                    tree->right_->color_ = rb_red;
                }
                else if (is_right_red(tree) && any_red_child(tree->right_)) { // symmetric to previous
                    if (is_left_red(tree->right_)) // case 2 - straightening zig-zag
                        rotate(tree->right_, rotate_right_tag{});
                    rotate(tree, rotate_left_tag{}); // case 3
                    //
                    tree->color_ = rb_black;
                    tree->left_->color_ = rb_red;
                }
            }
        }

        // Enables int type if red-black tree requirements are satisfied
        template <typename T, typename Tree, typename U = std::decay_t<T>>
        using enable_redblack_insert_t =
            std::enable_if_t< std::is_base_of_v<capability_insert_redblack, Tree> // only for capability_insert_redblack
                            && std::is_same_v<augment_type_t<Tree>, redblack_augment<U, node_type_t<Tree>>> // only for redblack_augment
                            && !std::is_base_of_v<capability_rotate, Tree> // rotation available on a user side may invalidate invariants
            , int>;
    }

    template <typename T, typename Tree, aux::enable_redblack_insert_t<T, Tree> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(aux::access(tree), capability_insert_redblack{}, std::forward<T>(v));
        aux::access(tree)->color_ = rb_black;
    }
    template <typename T, typename Tree, aux::enable_redblack_insert_t<T, Tree> = 0>
    void insert_or_assign(Tree& tree, T&& v) { // universal reference
        aux::insert_or_assign(aux::access(tree), capability_insert_redblack{}, std::forward<T>(v));
        aux::access(tree)->color_ = rb_black;
    }

    template <typename T, typename Tree, aux::enable_redblack_insert_t<T, Tree> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        // be mindful that initializers_list iterator produces refs to const, so move still leads to copies
        for (auto&& v : ls) insert(tree, std::move(v)); // call must go through insert with setting root to black
    }
    template <typename T, typename Tree, aux::enable_redblack_insert_t<T, Tree> = 0>
    void insert_or_assign(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) insert_or_assign(tree, std::move(v));
    }

    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert_redblack, Tree>, int> = 0>
    void insert(Tree& tree, It begin, It end) {
        while (begin != end) insert(tree, *begin++);
    }
    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert_redblack, Tree>, int> = 0>
    void insert_or_assign(Tree& tree, It begin, It end) {
        while (begin != end) insert_or_assign(tree, *begin++);
    }
}