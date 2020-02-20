/* Additional material
- https://brilliant.org/wiki/avl-tree/?subtopic=types-and-data-structures&chapter=binary-search-trees
- MIT Algorithms course https://www.youtube.com/watch?v=FNeL18KsWPc
*/
#pragma once

#include "capability_rotate.hpp"
#include "capability_insert.hpp"

#include <memory>
#include <algorithm>

namespace tree_search {

    struct capability_insert_avl {};

    namespace aux {

        struct avl_augment {
            int                height_ = -1;
        };

        template <typename Node>
        int def_height(std::unique_ptr<Node>& t) {
            return t ? t->augment_.height_ : -1;
        }

        template <typename Node>
        void update_height(std::unique_ptr<Node>& t) {
            if (t) t->augment_.height_ = 1 + std::max(def_height(t->left_), def_height(t->right_));
        }

        // is called from capability_insert
        template <typename Node>
        void fix_invariant(std::unique_ptr<Node>& t, capability_insert_avl) { // AVL invariant: abs(right - left) must be at most 1
            auto dif = def_height(t->right_) - def_height(t->left_);
            if (dif > 1) { // right subtree
                if (def_height(t->right_->left_) > def_height(t->right_->right_)) {
                    // case    \  
                    //         /
                    //
                    rotate(t->right_, rotate_right_tag{});
                    // update only two nodes
                    update_height(t->right_->right_);
                    update_height(t->right_);
                }
                // case        \
                    //              \ 
                    //
                rotate(t, rotate_left_tag{});
                // update only one node; current node will be processed further
                update_height(t->left_);
            }
            else if (dif < -1) {
                if (def_height(t->left_->right_) > def_height(t->left_->left_)) {
                    // case    /  
                    //         \
                        //
                    rotate(t->left_, rotate_left_tag{});
                    // update only two nodes
                    update_height(t->left_->left_);
                    update_height(t->left_);
                }
                // case        /
                //            / 
                //
                rotate(t, rotate_right_tag{});
                // update only one node; current node will be processed further
                update_height(t->right_);
            }
            update_height(t);
        }

        // Enables int type if avl tree requirements are satisfied
        template <typename T, typename Tree>
        using enable_avl_insert_t =
            std::enable_if_t< std::is_base_of_v<capability_insert_avl, Tree> // only for capability_insert_avl
                           && std::is_same_v<typename Tree::augment_type, aux::avl_augment> // only for avl_augment
                           && !std::is_base_of_v<capability_rotate, Tree> // rotation available on a user side may invalidate invariants
            , int>;
    }

    // TODO: verify against an existing element insertion
    // TODO: implement remove
    template <typename T, typename Tree, aux::enable_avl_insert_t<T, Tree> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(aux::access(tree), std::forward<T>(v), capability_insert_avl{});
    }

    template <typename T, typename Tree, aux::enable_avl_insert_t<T, Tree> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) insert(tree, std::move(v));
    }

}