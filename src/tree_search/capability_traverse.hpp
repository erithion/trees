#pragma once

#include "iterator_traverse.hpp"

namespace tree_search {
  
    template <typename It>
    struct iterators {
        It begin_;
        It end_ = {};
    };

    // for range loop support.
    // absence of begin/end methods in a range class r leads to ADL of begin/end functions and ADL expects them to be here.
    template<typename It>
    It begin(const iterators<It>& r) {
        return r.begin_;
    }
    template<typename It>
    It end(const iterators<It>& r) {
        return r.end_;
    }

    struct capability_traverse {};

    template <typename Tag, typename Tree, std::enable_if_t<std::is_base_of_v<capability_traverse, Tree>, int> = 0>
    auto traverse(Tag&& tag, const Tree& tree) {
        using iterator_type = tree_search::traverse_iterator<typename Tree::node_type, Tag>;
        return iterators<iterator_type>{ iterator_type(tree.root_.get()) };
    }
}
