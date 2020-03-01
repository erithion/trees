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
                if (tree->left_ && tree->right_) {
                    auto p = std::ref(tree->right_);
                    while (p.get()->left_) p = p.get()->left_;
                    std::swap(tree->value_, p.get()->value_);
                    remove(p.get(), v, tag);
                } 
                else if (tree->left_) {
                    std::swap(tree->value_, tree->left_->value_);
                    remove(tree->left_, v, tag);
                }
                else if (tree->right_) {
                    std::swap(tree->value_, tree->right_->value_);
                    remove(tree->right_, v, tag);
                }
                else {
                    tree = nullptr;
                    return;
                }
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