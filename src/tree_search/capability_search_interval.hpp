#pragma once 

#include "type_traverse.hpp"
#include "iterator.hpp"
#include "tree_interval.hpp"
#include "iterator_search_interval.hpp"

#include <type_traits>

namespace tree_search {

    struct capability_search_interval {};

    namespace aux {

        // Enables int type if interval tree requirements are satisfied
        template <typename Tag, typename T, typename Tree>
        using enable_interval_search_t =
            std::enable_if_t< std::is_base_of_v<capability_search_interval, Tree>
                                && aux::is_one_of_v<Tag, tag_inorder, tag_postorder, tag_preorder> // only for 3 predefined tags
                                && std::is_same_v<typename Tree::augment_type, aux::interval_augment<T>> // only for trees with interval_augment type within
                            , int>;
    }

    template <typename Tag, typename T, typename Tree, aux::enable_interval_search_t<Tag, T, Tree> = 0>
    auto search(Tag&& tag, const Tree& tree, const std::pair<T, T>& interval) {
        using iterator_type = tree_search::iterator_search_interval<typename Tree::node_type, Tag>;
        return iterators<iterator_type>{ iterator_type(aux::access(tree).get(), interval), iterator_type(interval) };
    }
}