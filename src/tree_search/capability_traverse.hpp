#pragma once

#include "type_traverse.hpp"

#include <functional>
#include <type_traits>
#include <memory>

namespace tree_search {

    struct capability_traverse {};

    namespace aux {

        template <typename Tree>
        void traverse(const std::unique_ptr<Tree>& tree, const std::function<void(const typename Tree::value_type&)>& fn, tag_inorder&&) {
            if (!tree) return;
            traverse(tree->left_, fn, tag_inorder{});
            if (fn) fn(tree->value_);
            traverse(tree->right_, fn, tag_inorder{});
        }

        template <typename Tree>
        void traverse(const std::unique_ptr<Tree>& tree, const std::function<void(const typename Tree::value_type)>& fn, tag_preorder&&) {
            if (!tree) return;
            if (fn) fn(tree->value_);
            traverse(tree->left_, fn, tag_preorder{});
            traverse(tree->right_, fn, tag_preorder{});
        }

        template <typename Tree>
        void traverse(const std::unique_ptr<Tree>& tree, const std::function<void(const typename Tree::value_type&)>& fn, tag_postorder&&) {
            if (!tree) return;
            traverse(tree->left_, fn, tag_postorder{});
            traverse(tree->right_, fn, tag_postorder{});
            if (fn) fn(tree->value_);
        }
    }

    template <typename Tag, typename Tree, std::enable_if_t<std::is_base_of_v<capability_traverse, Tree>, int> = 0>
    void traverse(Tag&& tag, const Tree& tree, const std::function<void(const typename Tree::value_type&)>& fn) {
        aux::traverse(tree.root_, fn, std::forward<Tag>(tag));
    }
}