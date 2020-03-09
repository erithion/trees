#pragma once

#include "type_capability.hpp"
#include "type_cti.hpp"

#include <memory> // std::unique_ptr
#include <type_traits>

namespace tree_search {
 
    namespace aux {

        template <typename Node>
        void fixup(std::unique_ptr<Node>& t, capability_insert) { } // for the cases when a tree has some invariants to restore

        struct tag_insert {};
        struct tag_assign {};

        template <typename T, typename Node>
        inline bool assign(std::unique_ptr<Node>& tree, tag_insert, T&& v) { return false; }

        template <typename T, typename Node>
        inline bool assign(std::unique_ptr<Node>& tree, tag_assign, T&& v) { 
            tree->value_ = std::forward<T>(v);
            return true;
        }

        template <typename T, typename Node, typename CapabilityTag, typename InsertTag, typename ... Aug>
        void insert(std::unique_ptr<Node>& tree, CapabilityTag ctag, InsertTag itag, T&& v, Aug&& ... pack) { // universal reference
            if (!tree) tree = std::make_unique<node_type_t<Node>>(std::forward<T>(v), std::forward<Aug>(pack)...);
            else if (v < tree->value_) insert(tree->left_, ctag, itag, std::forward<T>(v), std::forward<Aug>(pack)...);
            else if (v == tree->value_) { // found. now assign if it is assign_or_insert
                if (!assign(tree, itag, std::forward<T>(v))) return;
            }
            else if (!(v < tree->value_)) insert(tree->right_, ctag, itag, std::forward<T>(v), std::forward<Aug>(pack)...);
            fixup(tree, ctag);
        }
    }

    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert(Tree& tree, T&& v) { // universal reference
        aux::insert(aux::access(tree), capability_insert{}, aux::tag_insert{}, std::forward<T>(v));
    }
    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert_or_assign(Tree& tree, T&& v) { // universal reference
        aux::insert(aux::access(tree), capability_insert{}, aux::tag_assign{}, std::forward<T>(v));
    }

    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) aux::insert(aux::access(tree), capability_insert{}, aux::tag_insert{}, std::move(v));
    }
    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert_or_assign(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) aux::insert(aux::access(tree), capability_insert{}, aux::tag_assign{}, std::move(v));
    }

    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert(Tree& tree, It begin, It end) {
        while (begin != end) insert(tree, *begin++);
    }
    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_insert, Tree>, int> = 0>
    void insert_or_assign(Tree& tree, It begin, It end) {
        while (begin != end) insert_or_assign(tree, *begin++);
    }
}