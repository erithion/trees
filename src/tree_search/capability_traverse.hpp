#pragma once

#include "type_capability.hpp"
#include "iterator.hpp"

namespace tree_search {
  
    template <typename Tag, typename Tree, std::enable_if_t<std::is_base_of_v<capability_traverse, Tree>, int> = 0>
    auto traverse(Tag&& tag, const Tree& tree) {

        using iterator_type = tree_search::iterator<Tree, Tag>;

        return iterators<iterator_type>{ iterator_type(aux::access(tree).get()), iterator_type() };
    }
}
