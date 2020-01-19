/* Additional material
- https://brilliant.org/wiki/red-black-tree/?subtopic=types-and-data-structures&chapter=binary-search-trees
- MIT Course https://www.youtube.com/watch?v=O3hI9FdxFOM
- Visualisation for testing http://www.cs.armstrong.edu/liang/animation/web/RBTree.html
*/
#pragma once

#include "capability_rotate.hpp"
#include "capability_insert.hpp"

#include <memory>
#include <algorithm>

namespace tree_search {

    struct capability_insert_redblack {};

    namespace aux {

        struct redblack_augment {
            bool                is_red_ = true;
        };

        // Compliance within aux namespace is not checked in general. 
        // Just capability_insert_redblack is used on and off as a guard to prevent accidental overloading resolution calls
        template <typename Tree>
        bool is_left_red(const std::unique_ptr<Tree>& tree) {
            return tree && tree->left_ && tree->left_->augment_.is_red_;
        }

        template <typename Tree>
        bool is_right_red(const std::unique_ptr<Tree>& tree) {
            return tree && tree->right_ && tree->right_->augment_.is_red_;
        }

        template <typename Tree>
        bool any_red_child(const std::unique_ptr<Tree>& tree) {
            return is_left_red(tree) || is_right_red(tree);
        }

        template <typename Tree>
        bool any_red_grandchild(const std::unique_ptr<Tree>& tree) {
            if (!tree) return false;
            return is_left_red(tree->left_) || is_right_red(tree->left_)
                || is_left_red(tree->right_) || is_right_red(tree->right_);
        }

        /* Props of the red-black tree
        1. Each node is either red or black.
        2. The root is black.
        3. All leaves (NIL) are black.
        4. If a node is red, then its parent is black.
        5. Every path from a given node to any of its descendant NIL nodes contains the same number of black nodes (black-depth)
        6. The height of the red-black tree is at most 2 * log2(n + 1)
        */
        // is called from capability_insert
        template <typename Tree>
        void fix_invariant(std::unique_ptr<Tree>& tree, capability_insert_redblack) {
            if (tree && !tree->augment_.is_red_) {
                if (is_left_red(tree) && is_right_red(tree) && any_red_grandchild(tree)) { // Case 1 (left & right) -> recolor
                    tree->augment_.is_red_ = true;
                    tree->left_->augment_.is_red_ = false;
                    tree->right_->augment_.is_red_ = false;
                }
                else if (is_left_red(tree) && any_red_child(tree->left_)) {
                    if (is_right_red(tree->left_)) // case 2 - straightening zig-zag
                        rotate(tree->left_, rotate_left_tag{});
                    rotate(tree, rotate_right_tag{}); // case 3
                    //
                    tree->augment_.is_red_ = false;
                    tree->right_->augment_.is_red_ = true;
                }
                else if (is_right_red(tree) && any_red_child(tree->right_)) { // symmetric to previous
                    if (is_left_red(tree->right_)) // case 2 - straightening zig-zag
                        rotate(tree->right_, rotate_right_tag{});
                    rotate(tree, rotate_left_tag{}); // case 3
                    //
                    tree->augment_.is_red_ = false;
                    tree->left_->augment_.is_red_ = true;
                }
            }
        }

        // Enables int type if red-black tree requirements are satisfied
        template <typename T, typename Tree>
        using enable_redblack_insert_t =
            std::enable_if_t< std::is_base_of_v<capability_insert_redblack, Tree> // only for capability_insert_redblack
                           && std::is_same_v<typename Tree::augment_type, aux::redblack_augment> // only for redblack_augment
                           && !std::is_base_of_v<capability_rotate, Tree> // rotation available on a user side may invalidate invariants
            , int>;
    }

    template <typename T, typename Tree, aux::enable_redblack_insert_t<T, Tree> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(tree.root_, std::forward<T>(v), capability_insert_redblack{});
        tree.root_->augment_.is_red_ = false;
    }

    template <typename T, typename Tree, aux::enable_redblack_insert_t<T, Tree> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        // be mindful that initializers_list iterator produces pointers to const, so move still leads to copies
        for (auto&& v : ls) insert(tree, std::move(v)); // call must go through insert with setting root to black
    }
}