#pragma once 

#include <memory>
#include <vector>
#include <functional>
#include <type_traits>
#include <algorithm>

namespace tree_search {
    namespace aux {

        struct empty {};

        template <typename T, typename AugmentT>
        struct node {
            using value_type = T;
            using augment_type = AugmentT;
            using node_type = node<value_type, augment_type>;
            using ptr_type = std::unique_ptr<node_type>;

            using value_type_clean = std::decay_t<T>;//std::remove_reference_t<std::remove_cv_t<T>>;
            using augment_type_clean = std::decay_t<AugmentT>;//std::remove_reference_t<std::remove_cv_t<AugmentT>>;
            using node_type_clean = node<value_type_clean, augment_type_clean>;

            // TODO: add const to augment and value. add tests to check that; or private section
            value_type          value_;
            augment_type        augment_;
            ptr_type            left_;
            ptr_type            right_;

            node(const value_type& v)
                : value_(v), augment_{}, left_(nullptr), right_(nullptr) {}
            node(const value_type& v, const augment_type& a)
                : value_(v), augment_(a), left_(nullptr), right_(nullptr) {}

            node(value_type&& v, augment_type&& a) // no type deduction hence no universal reference, simple rvalue
                : value_(std::move(v)), augment_(std::move(a)), left_(nullptr), right_(nullptr) {}
            node(value_type&& v) // no type deduction hence no universal reference, simple rvalue
                : value_(std::move(v)), augment_{}, left_(nullptr), right_(nullptr) {}
        };
        
        template <typename Node>
        size_t size(const std::unique_ptr<Node>& tree) {
            if (!tree) return 0;
            return 1 + size(tree->left_) + size(tree->right_);
        }

        template <typename Node>
        int height(const std::unique_ptr<Node>& tree) {
            if (!tree) return 0;
            return 1 + std::max(height(tree->left_), height(tree->right_));
        }

        template <typename Node>
        std::pair<bool, int> perfect(const std::unique_ptr<Node>& tree) {  // max height of left must be greater than max height of right
            if (!tree) return std::make_pair(true, 0); // the flag signals if the node along with its subtrees has been perfect so far
                                                       // int contains the maximum height of the node along with its subtrees
            auto l = perfect(tree->left_);
            auto r = perfect(tree->right_);
            return std::make_pair(l.first && r.first && (l.second - r.second >= 0), 1 + std::max(l.second, r.second));
        }
    }

    template <typename T, typename AugmentT, typename ... Capabilities>
    struct tree : public Capabilities... { 
        using value_type = T;
        using augment_type = AugmentT;
        using node_type = aux::node<value_type, augment_type>;
        std::unique_ptr<node_type> root_;
    };

    template <typename ... pack>
    size_t size(const tree<pack...>& tree) {
        return aux::size(tree.root_);
    }

    template <typename ... pack>
    size_t height(const tree<pack...>& tree) {
        return aux::height(tree.root_);
    }

    template <typename ... pack>
    bool balanced(const tree<pack...>& tree) {
        if (!tree.root_) return true;
        return std::abs(aux::height(tree.root_->left_) - aux::height(tree.root_->right_)) <= 1;
    }

    template <typename ... pack>
    bool perfect(const tree<pack...>& tree) {
        return aux::perfect(tree.root_).first;
    }
}