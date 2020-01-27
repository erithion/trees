#pragma once 

#include "type_traverse.hpp"
#include "tree_interval.hpp"

#include <utility>

namespace tree_search {

    struct capability_search_interval {};

    namespace aux {

        template <typename Tree>
        using callback = std::function<void(const typename Tree::value_type&)>;

        template <typename T, typename Tree>
        void search(const std::unique_ptr<Tree>& tree, const std::pair<T, T>& interval, const callback<Tree>& fn, tag_inorder&&) {
            if (!tree) return;
            if (tree->left_ && tree->left_->augment_.max_ >= interval.first)
                search(tree->left_, interval, fn, tag_inorder{});
            if (interval.first <= tree->value_.second && !(interval.second < tree->value_.first))
                if (fn) fn(tree->value_);
            if (tree->value_.first <= interval.second)
                search(tree->right_, interval, fn, tag_inorder{});
        }

        template <typename T, typename Tree>
        void search(const std::unique_ptr<Tree>& tree, const std::pair<T, T>& interval, const callback<Tree>& fn, tag_preorder&&) {
            if (!tree) return;
            if (interval.first <= tree->value_.second && !(interval.second < tree->value_.first))
                if (fn) fn(tree->value_);
            if (tree->left_ && tree->left_->augment_.max_ >= interval.first)
                search(tree->left_, interval, fn, tag_inorder{});
            if (tree->value_.first <= interval.second)
                search(tree->right_, interval, fn, tag_inorder{});
        }

        template <typename T, typename Tree>
        void search(const std::unique_ptr<Tree>& tree, const std::pair<T, T>& interval, const callback<Tree>& fn, tag_postorder&&) {
            if (!tree) return;
            if (tree->left_ && tree->left_->augment_.max_ >= interval.first)
                search(tree->left_, interval, fn, tag_inorder{});
            if (tree->value_.first <= interval.second)
                search(tree->right_, interval, fn, tag_inorder{});
            if (interval.first <= tree->value_.second && !(interval.second < tree->value_.first))
                if (fn) fn(tree->value_);
        }

        template <typename T, typename ... Ts>
        constexpr bool is_one_of_v = std::disjunction_v<std::is_same<T, Ts>...>;

        // Enables int type if interval tree requirements are satisfied
        template <typename Tag, typename T, typename Tree>
        using enable_interval_search_t =
            std::enable_if_t< std::is_base_of_v<capability_search_interval, Tree>
                           && aux::is_one_of_v<Tag, tag_inorder, tag_postorder, tag_preorder> // only for 3 predefined tags
                           && std::is_same_v<typename Tree::augment_type, aux::interval_augment<T>> // only for trees with interval_augment type within
            , int>;
    }

    // TODO: implement iterators for searching
    template <typename Tag, typename T, typename Tree, aux::enable_interval_search_t<Tag, T, Tree> = 0>
    void search(Tag&& tag, const Tree& tree, const std::pair<T, T>& interval, const std::function<void(const typename Tree::value_type&)>& fn) {
        aux::search(tree.root_, interval, fn, std::move(tag));
    }
}