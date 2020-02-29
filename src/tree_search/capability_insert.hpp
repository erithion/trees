#pragma once

#include "type_capability.hpp"

#include <memory>
#include <type_traits>

namespace tree_search {
 
    namespace aux {

        template <typename Node>
        void fix_invariant(std::unique_ptr<Node>& t, capability_insert) { }

        template <typename T, typename Node, typename Tag>
        void insert(std::unique_ptr<Node>& tree, T&& v, const Tag& tag) { // universal reference
            using type = std::decay_t<typename Node::node_type>;
            if (!tree) tree = std::make_unique<type>(std::forward<T>(v));
            else if (v < tree->value_) insert(tree->left_, std::forward<T>(v), tag);
            else if (v > tree->value_) insert(tree->right_, std::forward<T>(v), tag);
            fix_invariant(tree, tag);
        }
    }

    // TODO: check/resolve an existing element insertion
    // TODO: implement remove
    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(aux::access(tree), std::forward<T>(v), capability_insert{});
    }

    // TODO: consider inserting from std containers
    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) aux::insert(aux::access(tree), std::move(v), capability_insert{});
    }
}