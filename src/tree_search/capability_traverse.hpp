#pragma once

#include "iterator_traverse.hpp"

namespace tree_search {
  
    template <typename It>
    struct iterators {
        It begin;
        It end = {};
    };

    struct capability_traverse {};

    template <typename Tag, typename Tree, std::enable_if_t<std::is_base_of_v<capability_traverse, Tree>, int> = 0>
    auto traverse(Tag&& tag, const Tree& tree) {
        using iterator_type = tree_search::traverse_iterator<typename Tree::node_type, Tag>;
        return iterators<iterator_type>{ iterator_type(tree.root_.get()) };
    }
}