#pragma once

#include "type_capability.hpp"
#include "capability_rotate.hpp"
#include "type_cti.hpp"

#include <type_traits>
#include <algorithm> // std::max
#include <memory> // std::unique_ptr

namespace tree_search {

    enum rb_color : bool {
        rb_black = false,
        rb_red = true
    };

    template <typename, typename>
    struct redblack_augment {
        rb_color color_ = rb_red;
    };

    namespace aux {

        template <typename Node>
        int black_height(const std::unique_ptr<Node>& tree) { // returns 0 if the tree is not rb-balanced
            if (!tree) return 1;
            auto lf = black_height(tree->left_);
            auto rh = black_height(tree->right_);
            auto cn = tree->color_ == rb_black ? 1 : 0;
            return lf == rh ? lf + cn : 0;
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
        return aux::black_height(root) != 0 && root->color_ == rb_black;
    }
}