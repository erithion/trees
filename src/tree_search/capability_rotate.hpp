#pragma once

#include "type_capability.hpp"

#include <memory>
#include <type_traits>

namespace tree_search {

    struct rotate_left_tag {};
    struct rotate_right_tag {};

    // versions for a node-like tree
    template <typename Node>
    void rotate(std::unique_ptr<Node>& cur, rotate_right_tag&&) {
        if (!cur) return;
        auto left = std::move(cur->left_); // cur's parent ok; left's right and left are empty :( they were bound to cur->left
        cur->left_ = std::move(left->right_); // left->rights's parent points on self because it was bound previously to cur->left
        left->right_ = std::move(cur);
        cur = std::move(left);
    }
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