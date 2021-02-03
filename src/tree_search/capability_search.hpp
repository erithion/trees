#pragma once 

#include "type_capability.hpp"
#include "type_traverse.hpp"
#include "iterator.hpp"

#include <functional> // std::function
#include <type_traits>

namespace tree_search {

    template <typename Tree>
    using predicate = std::function<bool(const aux::value_type_t<Tree>&)>;

    namespace aux {

        template <typename Tree>
        struct search_condition {

            search_condition() = default;
            explicit search_condition(const predicate<Tree>& f) : fn_(f) {}
            inline bool operator()(const aux::node_type_t<Tree>* p) const {
                return this->fn_ && this->fn_(p->value_);
            }
            predicate<Tree> fn_;
        };

        template <typename Tree>
        struct search_truth {

            search_truth() = default;
            explicit search_truth(const predicate<Tree>&) {}
            inline bool operator()(const aux::node_type_t<Tree>* p) const { return true; }
        };

        // Enables int type if interval tree requirements are satisfied
        template <typename Tag, typename Tree>
        using enable_search_t =
            std::enable_if_t< std::is_base_of_v<capability_search, Tree>
                                && aux::is_one_of_v<Tag, tag_inorder, tag_postorder, tag_preorder> // only for 3 predefined tags
                                && !std::is_base_of_v<capability_search_interval, Tree> // interval tree has its own search
                            , int>;
    }

    template <typename Tag, typename Tree, aux::enable_search_t<Tag, Tree> = 0>
    auto search(Tag&& tag, const Tree& tree, const predicate<Tree>& fn) {

        using cond = aux::search_condition<Tree>;
        using truth = aux::search_truth<Tree>;
        using iterator_type = tree_search::iterator<Tree, Tag, cond, truth, truth>;

        return iterators<iterator_type>{ iterator_type(aux::access(tree).get(), fn), iterator_type() };
    }
}