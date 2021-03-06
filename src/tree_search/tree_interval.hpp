/* Additional material
- https://www.geeksforgeeks.org/interval-tree/
*/
#pragma once

#include "tree_redblack.hpp"
#include "capability_insert_redblack.hpp"
#include "capability_remove_redblack.hpp"
#include "type_cti.hpp" 

#include <limits> // min
#include <utility> // std::pair
#include <memory> // std::unique_ptr
#include <algorithm> // std::max

namespace tree_search {

    template <typename Interval, typename Node>
    struct interval_augment; // add specializations for the types you need. std::pair below is an example.

    template <typename T, typename Node>
    struct interval_augment<std::pair<T, T>, Node>
        : public redblack_augment<std::pair<T, T>, Node> {
        using base_type = T;
        base_type   max_ = std::numeric_limits<base_type>::min();

        template <typename U = base_type>
        explicit interval_augment(U&& u) : max_(std::forward<U>(u)) {}
    };

    namespace aux {

        // Enables int type if interval tree requirements are satisfied
        template <typename Interval, typename Tree
            , typename Augment = typename Tree::augment_type
            , typename U = std::decay_t<Interval>> // local aliases, sort of
        using enable_interval_insert_t =
            std::enable_if_t< std::is_base_of_v<capability_insert_interval, Tree> // only for capability_insert_interval
                        && std::is_same_v<Augment, interval_augment<U, node_type_t<Tree>>> // only for interval_augment
                        && !std::is_base_of_v<capability_rotate, Tree> // rotation available on a user side may invalidate invariants
            , int>;

        template <typename Interval, typename Tree
            , typename Augment = typename Tree::augment_type
            , typename U = std::decay_t<Interval>> // local aliases, sort of
        using enable_interval_remove_t =
            std::enable_if_t< std::is_base_of_v<capability_remove_interval, Tree> // only for capability_remove_interval
                        && std::is_same_v<Augment, interval_augment<U, node_type_t<Tree>>> // only for interval_augment
                        && !std::is_base_of_v<capability_rotate, Tree> // rotation available on a user side may invalidate invariants
            , int>;

        // Compliance within aux namespace is not checked in general. 
        // Just capability_insert_interval is used as a guard to prevent accidental overloading resolution calls
        template <typename Node>
        void update_max(std::unique_ptr<Node>& tree) {
            if (!tree) return;
            tree->max_ = tree->value_.second;
            if (tree->left_)
                tree->max_ = std::max(tree->left_->max_, tree->max_);
            if (tree->right_)
                tree->max_ = std::max(tree->right_->max_, tree->max_);
        }

        // called from capability_insert
        template <typename Node>
        void fixup(std::unique_ptr<Node>& tree, capability_insert_interval) {
            aux::fixup(tree, capability_insert_redblack{});
            update_max(tree->left_);
            update_max(tree->right_);
            update_max(tree);
        }

        // called from capability_remove_redblack
        template <typename Node>
        void fixup(std::unique_ptr<Node>& tree, capability_remove_interval) {
            update_max(tree->left_);
            update_max(tree->right_);
            update_max(tree);
        }
    }

    // interval is a finite type which makes it unique, hence no need in insert_or_assign. just insert is enough.

    template <typename T, typename Tree, aux::enable_interval_insert_t<T, Tree> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(aux::access(tree), capability_insert_interval{}, std::forward<T>(v), v.second);
        aux::access(tree)->color_ = rb_black;
    }

    template <typename T, typename Tree, aux::enable_interval_insert_t<T, Tree> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        // be mindful that initializers_list iterator produces pointers to const, so move still leads to copies
        for (auto&& v : ls) insert(tree, std::move(v)); // call must go through insert with setting root to black
    }

    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert_interval, Tree>, int> = 0>
    void insert(Tree& tree, It begin, It end) {
        while (begin != end) insert(tree, *begin++);
    }

    template <typename T, typename Tree, aux::enable_interval_remove_t<T, Tree> = 0>
    void remove(Tree& tree, const T& v) {
        aux::rbremove(aux::access(tree), v, capability_remove_interval{});
    }

    template <typename T, typename Tree, aux::enable_interval_remove_t<T, Tree> = 0>
    void remove(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) remove(tree, v);
    }

    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_remove_interval, Tree>, int> = 0>
    void remove(Tree& tree, It begin, It end) {
        while (begin != end) remove(tree, *begin++);
    }
}