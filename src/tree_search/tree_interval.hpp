/* Additional material
- https://www.geeksforgeeks.org/interval-tree/
*/
#pragma once

#include "tree_redblack.hpp"
#include "capability_redblack_insert.hpp"
#include "type_cti.hpp" 

#include <limits>
#include <utility>
#include <memory>
#include <algorithm>

namespace tree_search {

    struct capability_insert_interval {};

    namespace aux {

        // TODO: add interval tests for characters
        template <typename Interval, typename Node>
        struct interval_augment; // add specializations for the types you need. std::pair below is an example.

        template <typename T, typename Node>
        struct interval_augment<std::pair<T, T>, Node>
            : public aux::redblack_augment<std::pair<T, T>, Node> {
            using base_type = T;
            base_type   max_ = std::numeric_limits<base_type>::min();
        };

        // Enables int type if interval tree requirements are satisfied
        template <typename Interval, typename Tree
            , typename Augment = typename Tree::augment_type> // local aliases, sort of
        using enable_interval_insert_t =
            std::enable_if_t< std::is_base_of_v<capability_insert_interval, Tree> // only for capability_insert_interval
                        && std::is_same_v<Augment, aux::interval_augment<std::decay_t<Interval>, aux::node_type_t<Tree>>> // only for interval_augment
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

        template <typename Interval, typename Node>
        void insert(std::unique_ptr<Node>& tree, Interval&& v, capability_insert_interval) {
            using type = std::decay_t<typename Node::node_type>;
            if (!tree) {
                tree = std::make_unique<type>(std::forward<Interval>(v));
                tree->max_ = v.second;
            }
            else if (v < tree->value_) insert(tree->left_, std::forward<Interval>(v), capability_insert_interval{});
            else if (v > tree->value_) insert(tree->right_, std::forward<Interval>(v), capability_insert_interval{});
            aux::fix_invariant(tree, capability_insert_redblack{});
            update_max(tree->left_);
            update_max(tree->right_);
            update_max(tree);
        }
    }

    // TODO: check/resolve insertion of an existing interval
    // TODO: implement remove
    template <typename T, typename Tree, aux::enable_interval_insert_t<T, Tree> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(aux::access(tree), std::forward<T>(v), capability_insert_interval{});
        aux::access(tree)->color_ = aux::rb_black;
    }

    template <typename T, typename Tree, aux::enable_interval_insert_t<T, Tree> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        // be mindful that initializers_list iterator produces pointers to const, so move still leads to copies
        for (auto&& v : ls) insert(tree, std::move(v)); // call must go through insert with setting root to black
    }
}