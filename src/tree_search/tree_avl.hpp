/* Additional material
- https://brilliant.org/wiki/avl-tree/?subtopic=types-and-data-structures&chapter=binary-search-trees
- MIT Algorithms course https://www.youtube.com/watch?v=FNeL18KsWPc

- https://stackoverflow.com/questions/3955680/how-to-check-if-my-avl-tree-implementation-is-correct deletion test cases
- http://www.mathcs.emory.edu/~cheung/Courses/323/Syllabus/Trees/AVL-delete.html 
- 
*/
#pragma once

#include "capability_rotate.hpp"
#include "capability_insert.hpp"
#include "capability_remove.hpp"
#include "type_capability.hpp"
#include "type_cti.hpp"

#include <memory> // std::unique_ptr
#include <algorithm> // std::max

namespace tree_search {

    template <typename, typename>
    struct avl_augment {
        int height_ = -1;
    };

    namespace aux {

        template <typename Node>
        int def_height(std::unique_ptr<Node>& t) {
            return t ? t->height_ : -1;
        }

        template <typename Node>
        void update_height(std::unique_ptr<Node>& t) {
            if (t) t->height_ = 1 + std::max(def_height(t->left_), def_height(t->right_));
        }

        // is called from capability_insert/capability_remove
        template <typename Node, typename Tag, std::enable_if_t<aux::is_one_of_v<Tag, capability_insert_avl, capability_remove_avl>, int> = 0>
        void fixup(std::unique_ptr<Node>& t, Tag) { // AVL invariant: abs(right - left) must be at most 1
            auto dif = def_height(t->right_) - def_height(t->left_);
            if (dif > 1) { // right subtree
                if (def_height(t->right_->left_) > def_height(t->right_->right_)) {
                    // case right->left
                    rotate(t->right_, rotate_right_tag{});
                    // update only two nodes
                    update_height(t->right_->right_);
                    update_height(t->right_);
                }
                // case right->right
                rotate(t, rotate_left_tag{});
                // update only one node; current node will be processed further
                update_height(t->left_);
            }
            else if (dif < -1) {
                if (def_height(t->left_->right_) > def_height(t->left_->left_)) {
                    // case left->right
                    rotate(t->left_, rotate_left_tag{});
                    // update only two nodes
                    update_height(t->left_->left_);
                    update_height(t->left_);
                }
                // case left->left
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
                           && std::is_same_v<augment_type_t<Tree>, avl_augment<std::decay_t<T>, node_type_t<Tree>>> // only for avl_augment
                           && !std::is_base_of_v<capability_rotate, Tree> // rotation available on a user side may invalidate invariants
            , int>;

        template <typename T, typename Tree>
        using enable_avl_remove_t =
            std::enable_if_t< std::is_base_of_v<capability_remove_avl, Tree> // only for capability_remove_avl
                            && std::is_same_v<augment_type_t<Tree>, avl_augment<std::decay_t<T>, node_type_t<Tree>>> // only for avl_augment
                            && !std::is_base_of_v<capability_rotate, Tree> // rotation available on a user side may invalidate invariants
            , int>;
    }

    template <typename T, typename Tree, aux::enable_avl_insert_t<T, Tree> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(aux::access(tree), capability_insert_avl{}, std::forward<T>(v));
    }

    template <typename T, typename Tree, aux::enable_avl_insert_t<T, Tree> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) insert(tree, std::move(v));
    }

    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert_avl, Tree>, int> = 0>
    void insert(Tree& tree, It begin, It end) {
        while (begin != end) insert(tree, *begin++);
    }

    template <typename T, typename Tree, aux::enable_avl_insert_t<T, Tree> = 0>
    void insert_or_assign(Tree& tree, T&& v) { // universal reference
        aux::insert_or_assign(aux::access(tree), capability_insert_avl{}, std::forward<T>(v));
    }

    template <typename T, typename Tree, aux::enable_avl_insert_t<T, Tree> = 0>
    void insert_or_assign(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) insert_or_assign(tree, std::move(v));
    }

    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert_avl, Tree>, int> = 0>
    void insert_or_assign(Tree& tree, It begin, It end) {
        while (begin != end) insert_or_assign(tree, *begin++);
    }

    template <typename T, typename Tree, aux::enable_avl_remove_t<T, Tree> = 0>
    void remove(Tree& tree, const T& v) {
        aux::remove(aux::access(tree), v, capability_remove_avl{});
    }

    template <typename T, typename Tree, aux::enable_avl_remove_t<T, Tree> = 0>
    void remove(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) remove(tree, v);
    }

    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_remove_avl, Tree>, int> = 0>
    void remove(Tree& tree, It begin, It end) {
        while (begin != end) remove(tree, *begin++);
    }
}