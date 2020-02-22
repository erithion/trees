#pragma once

#include "type_capability.hpp"

#include <memory>
#include <type_traits>
#include <stack>

namespace tree_search {
 
    namespace aux {

        template <typename Node>
        void fix_invariant(std::unique_ptr<Node>& t, capability_remove) { }

        template <typename T, typename Node, typename Tag>
        void remove(std::unique_ptr<Node>& tree, const T& v, const Tag& tag) {
            if (!tree) return;
            else if (v < tree->value_) remove(tree->left_, v, tag);
            else if (v > tree->value_) remove(tree->right_, v, tag);
            else { // found
                auto p = std::ref(tree);
                if (tree->left_ && tree->right_) {
                    for(p = tree->right_; p.get()->left_; p = p.get()->left_);
                    tree->value_ = std::move(p.get()->value_);
                } 
                else if (tree->left_) {
                    tree->value_ = std::move(tree->left_->value_);
                    p = tree->left_;
                }
                else if (tree->right_) {
                    tree->value_ = std::move(tree->right_->value_);
                    p = tree->right_;
                }
                else {
                    tree = nullptr;
                    return;
                }
                remove(p.get(), p.get()->value_, tag);
            }
            fix_invariant(tree, tag);
        }
    }

    // TODO: check/resolve an existing element insertion
    // TODO: implement remove
    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_remove, Tree>, int> = 0>
    void remove(Tree& tree, const T& v) {
        aux::remove(aux::access(tree), v, capability_remove{});
    }

    // TODO: consider remove from std containers
    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_remove, Tree>, int> = 0>
    void remove(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) aux::remove(aux::access(tree), std::move(v), capability_remove{});
    }
}