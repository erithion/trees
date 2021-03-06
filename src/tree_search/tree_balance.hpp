/* Additional material
- https://stackoverflow.com/questions/14001676/balancing-a-bst
- Day-Warren-Stout algorithm http://penguin.ewu.edu/~trolfe/DSWpaper/
*/
#pragma once

#include "capability_insert.hpp"
#include "capability_rotate.hpp"

#include <memory> // std::unique_ptr
#include <type_traits> // std::ref

namespace tree_search {

    namespace aux {
        // Transforms a tree into a right degenerate tree (or "vine" according to the Warren-Stout algorithm's notation)
        template <typename Node>
        size_t to_right_list(std::unique_ptr<Node>& tree) {
            size_t size = 0;
            auto   cur = std::ref(tree);
            while (cur.get())
                if (cur.get()->left_) rotate(cur.get(), rotate_right_tag{});
                else ++size, cur = std::ref(cur.get()->right_);
            return size;
        }

        // Enables int type if manual balance tree requirements are satisfied
        template <typename Tree>
        using enable_manual_balance_t =
            std::enable_if_t< std::is_base_of_v<capability_balance_manual, Tree>
                            && (std::is_base_of_v<capability_insert, Tree>
                                   || std::is_base_of_v<capability_remove, Tree>) // to disable balance for auto-balancing trees
                            
            , int>;
    }

    // Re-balance a tree to a COMPLETE tree by the Warren-Stout algorithm 
    template <typename Tree, aux::enable_manual_balance_t<Tree> = 0>
    void balance(Tree& tree) {
        auto size = aux::to_right_list(aux::access(tree));
        // We want the highest FULL tree size not bigger than the current size: n_full_highest <= size
        // Geometric progression gives n_full = 2^0 + 2^1 + 2^2 + ... + 2^k = 2^(k+1)-1 = 2^height - 1 => n_full + 1 = 2^height
        // Since any FULL tree size ultimately results in powers of 2, 
        // it's enough to find out the lower (power of 2) border of the current nonfull tree size
        // log2 and floor will do the trick
        auto height = static_cast<size_t>(std::floor(std::log2(size + 1)));
        auto n_full = static_cast<size_t>(std::pow(2, height) - 1); // -1 is due to the above formula for n_full 
        // All that in order to generate a COMPLETE tree, i.e. leftovers beyond n_full must be the leftmost elements of the tree
        // So we left-rotate the leftovers first
        auto leftovers_count = size - n_full;
        auto cur = std::ref(aux::access(tree));
        for (auto j = 0; j != leftovers_count; ++j) {
            rotate(cur.get(), rotate_left_tag{});
            cur = std::ref(cur.get()->right_);
        }
        // Now just balance
        for (auto i = n_full / 2; i != 0; i /= 2) {
            auto cur = std::ref(aux::access(tree));
            for (auto j = 0; j != i; ++j) {
                rotate(cur.get(), rotate_left_tag{});
                cur = std::ref(cur.get()->right_);
            }
        }
    }
}