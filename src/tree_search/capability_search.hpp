#pragma once 

#include "type_traverse.hpp"
#include "iterator.hpp"
#include "iterator_search.hpp"

#include <type_traits>

namespace tree_search {

    struct capability_search {};

    namespace aux {

        // Enables int type if interval tree requirements are satisfied
        template <typename Tag, typename Tree>
        using enable_search_t =
            std::enable_if_t< std::is_base_of_v<capability_search, Tree>
                                && aux::is_one_of_v<Tag, tag_inorder, tag_postorder, tag_preorder> // only for 3 predefined tags
                            , int>;
    }

    template <typename T>
    using is = bool (const T&);

    template <typename Tag, typename Tree, aux::enable_search_t<Tag, Tree> = 0>
    auto search(Tag&& tag, const Tree& tree, const std::function<bool(const typename Tree::value_type&)>& fn) {
        using iterator_type = tree_search::iterator_search<typename Tree::node_type, Tag>;
        auto&& f = [fn](const typename Tree::node_type* p) { return fn(p->value_); };
        return iterators<iterator_type>{ iterator_type(aux::access(tree).get(), std::move(f)), iterator_type() };
    }
}