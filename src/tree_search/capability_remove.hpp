#pragma once

#include "type_capability.hpp"

#include <memory>
#include <type_traits>
#include <stack>

namespace tree_search {
 
    namespace aux {

        template <typename Node>
        void fixup(std::unique_ptr<Node>& t, capability_remove) { } // for the cases when a tree has some invariants to restore

        template <typename T, typename Node, typename Tag>
        void remove(std::unique_ptr<Node>& tree, const T& v, Tag tag) {
            if (!tree) return;
            else if (v < tree->value_) remove(tree->left_, v, tag);
            else if (v > tree->value_) remove(tree->right_, v, tag);
            else { // found
                if (tree->left_ && tree->right_) {
                    auto ptr = tree->right_.get();
                    while (ptr->left_) ptr = ptr->left_.get();
                    std::swap(tree->value_, ptr->value_);
                    remove(tree->right_, v, tag); // going down is not necessary for BSTs, but might be for AVLs or RB-trees
                                                  // it's still log_2(n) deep, though: 
                                                  // in a tree with 10^15 (quadrillion) nodes there would be at most 50 recursive calls. 
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
            fixup(tree, tag);
        }
    }

    // TODO: check/resolve an existing element insertion
    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_remove, Tree>, int> = 0>
    void remove(Tree& tree, const T& v) {
        aux::remove(aux::access(tree), v, capability_remove{});
    }

    template <typename T, typename Tree, std::enable_if_t<std::is_base_of_v<capability_remove, Tree>, int> = 0>
    void remove(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) aux::remove(aux::access(tree), std::move(v), capability_remove{});
    }

    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_remove, Tree>, int> = 0>
    void remove(Tree& tree, It begin, It end) {
        while (begin != end) remove(tree, *begin++);
    }
}