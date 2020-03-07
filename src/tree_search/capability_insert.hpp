#pragma once

#include "type_capability.hpp"
#include "type_cti.hpp"

#include <memory>
#include <type_traits>

namespace tree_search {
 
    namespace aux {

        template <typename Node>
        void fixup(std::unique_ptr<Node>& t, capability_insert) { } // for the cases when a tree has some invariants to restore

        template <typename T, typename Node, typename Tag, typename ... Aug>
        void insert(std::unique_ptr<Node>& tree, Tag tag, T&& v, Aug&& ... pack) { // universal reference
            if (!tree) tree = std::make_unique<node_type_t<Node>>(std::forward<T>(v), std::forward<Aug>(pack)...);
            else if (v < tree->value_) insert(tree->left_, tag, std::forward<T>(v), std::forward<Aug>(pack)...);
            else if (v > tree->value_) insert(tree->right_, tag, std::forward<T>(v), std::forward<Aug>(pack)...);
            fixup(tree, tag);
        }
    }

    // TODO: check/resolve an existing element insertion
    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(aux::access(tree), capability_insert{}, std::forward<T>(v));
    }

    // TODO: consider inserting from std containers
    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) aux::insert(aux::access(tree), capability_insert{}, std::move(v));
    }
}