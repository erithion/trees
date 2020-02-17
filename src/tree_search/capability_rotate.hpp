#pragma once

#include <memory>
#include <functional>
#include <type_traits>

namespace tree_search {

    struct capability_rotate {};

    struct rotate_left_tag {};
    struct rotate_right_tag {};

    // version for a node-like tree
    template <typename Node>
    void rotate(std::unique_ptr<Node>& cur, rotate_right_tag&&) {
        if (!cur) return;
        auto left = std::move(cur->left_);
        cur->left_ = std::move(left->right_);
        left->right_ = std::move(cur);
        cur = std::move(left);
    }
    // version for a node-like tree
    template <typename Node>
    void rotate(std::unique_ptr<Node>& cur, rotate_left_tag&&) {
        if (!cur) return;
        auto right = std::move(cur->right_);
        cur->right_ = std::move(right->left_);
        right->left_ = std::move(cur);
        cur = std::move(right);
    }

    // version for the outermost tree container
    template <typename Tag, typename Tree, std::enable_if_t<std::is_base_of_v<capability_rotate, Tree>, int> = 0>
    void rotate(Tree& tree, Tag&& tag) { // universal reference
        rotate(aux::access(tree), std::forward<Tag>(tag));
    }
}