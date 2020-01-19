#pragma once

#include <memory>
#include <functional>
#include <type_traits>

namespace tree_search {
 
    struct capability_insert {};
    namespace aux {

        template <typename Tree>
        void fix_invariant(std::unique_ptr<Tree>& t, capability_insert) { }

        template <typename T, typename Tree, typename Tag>
        void insert(std::unique_ptr<Tree>& tree, T&& v, const Tag& tag) { // universal reference
            if (!tree) tree = std::make_unique<typename Tree::node_type_clean>(std::forward<T>(v));
            else if (v < tree->value_) insert(tree->left_, std::forward<T>(v), tag);
            else if (v > tree->value_) insert(tree->right_, std::forward<T>(v), tag);
            fix_invariant(tree, tag);
        }
    }

    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(tree.root_, std::forward<T>(v), capability_insert{});
    }

    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) aux::insert(tree.root_, std::move(v), capability_insert{});
    }
}