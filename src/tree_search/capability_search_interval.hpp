#pragma once 

#include "type_capability.hpp"
#include "type_traverse.hpp"
#include "iterator.hpp"
#include "tree_interval.hpp"

#include <type_traits>

namespace tree_search {

    namespace aux {

        struct spec_left {};
        struct spec_right {};
        struct spec_cur {};

        //how iterator would detect intersections
        template <typename Tree, typename SpecTag>
        struct interval_intersect {

            interval_intersect() = default;
            explicit interval_intersect(const aux::value_type_t<Tree>& bounds) : bounds_(bounds) {}

            template <typename U = SpecTag, std::enable_if_t<std::is_same_v<U, spec_left>, int> = 0>
            inline bool operator()(const aux::node_type_t<Tree>* p) const { // left child's max border MIGHT get smaller with each next step down the tree 
                return p->left_ && p->left_->max_ >= this->bounds_.first;
            }

            template <typename U = SpecTag, std::enable_if_t<std::is_same_v<U, spec_right>, int> = 0>
            inline bool operator()(const aux::node_type_t<Tree>* p) const { // right child's left border gets ever bigger with each next step down the tree 
                return p->right_ && p->right_->value_.first <= this->bounds_.second;
            }

            template <typename U = SpecTag, std::enable_if_t<std::is_same_v<U, spec_cur>, int> = 0>
            inline bool operator()(const aux::node_type_t<Tree>* p) const { // covers all 3 possible cases of intersection with the current node
                return this->bounds_.first <= p->value_.second && this->bounds_.second >= p->value_.first;
            }
            aux::value_type_t<Tree> bounds_ = {};
        };

        // Enables int type if interval tree requirements are satisfied
        template <typename Tag, typename Interval, typename Tree>
        using enable_interval_search_t =
            std::enable_if_t<std::is_base_of_v<capability_search_interval, Tree>
                && std::is_same_v<augment_type_t<Tree>, interval_augment<Interval, node_type_t<Tree>>> // only for trees with interval_augment type within
            , int>;
    }

    template <typename Tag, typename Interval, typename Tree, aux::enable_interval_search_t<Tag, Interval, Tree> = 0>
    auto search(Tag&& tag, const Tree& tree, const Interval& interval) {

        using left = aux::interval_intersect<Tree, aux::spec_left>;
        using right = aux::interval_intersect<Tree, aux::spec_right>;
        using cur = aux::interval_intersect<Tree, aux::spec_cur>;
        using iterator_type = tree_search::iterator<Tree, Tag, cur, left, right>;

        return iterators<iterator_type>{ iterator_type(aux::access(tree).get(), interval), iterator_type() };
    }
}