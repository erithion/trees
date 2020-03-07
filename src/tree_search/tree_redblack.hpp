#pragma once

#include "type_capability.hpp"
#include "capability_rotate.hpp"
#include "type_cti.hpp"

#include <type_traits>
#include <algorithm> // std::max
#include <memory> // std::unique_ptr

namespace tree_search {

    namespace aux {

        enum rb_color : bool {
            rb_black = false,
            rb_red = true
        };

        template <typename, typename>
        struct redblack_augment {
            rb_color color_ = rb_red;
        };

        template <typename Node>
        int black(const std::unique_ptr<Node>& tree) {
            if (!tree) return 0;
            // TODO: needs a formal argument for std::max that would prove the correctness of the algorithm
            return (tree->color_ == rb_black ? 1 : 0) + std::max(black(tree->left_), black(tree->right_));
        }

        template <typename Tree>
        using enable_redblack_optimal_t =
                std::enable_if_t<std::is_base_of_v<redblack_augment<value_type_t<Tree>, node_type_t<Tree>>, augment_type_t<Tree>> // only for redblack_augment and descendants
            , int>;
    }

    template <typename Tree, aux::enable_redblack_optimal_t<Tree> = 0>
    bool optimal(const Tree& tree) { // RB-tree is not balanced in a general sense. 
                                     // Yet if it maintains its invariants, it guarantees log_2(n) search/insert/remove complexity.
        auto&& root = aux::access(tree);
        if (!root) return true;
        return aux::black(root->left_) == aux::black(root->right_);
    }
}