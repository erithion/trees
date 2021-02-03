#pragma once 

#include "tree_search/type_compatibility.hpp"

#include <memory> // unique_ptr
#include <type_traits>
#include <algorithm> // max

namespace tree_search {

    namespace aux {

        template <typename Element, template <typename /*Element*/, typename /*Node*/> typename Augment>
        struct node 
            : public Augment<Element, node<Element, Augment>> { // employing Curiously Recurring Template Pattern
            using value_type = Element;
            using augment_type = Augment<Element, node>;
            using node_type = node;
            using ptr_type = std::unique_ptr<node_type>;

            value_type          value_; 
            ptr_type            left_;
            ptr_type            right_;

            // By using templated constructors we force the compiler to perform type deduction,
            // so that it would apply the rule of universal references to the deducted value_type and augmented_type.
            // Otherwise we would have to implement pair of separate constructors for rvalue and lvalue
            template <typename V = value_type, typename ... AugPack>
            explicit node(V&& v, AugPack&&... p)
                : augment_type(std::forward<AugPack>(p)...), value_(std::forward<V>(v)), left_(nullptr), right_(nullptr) {}
        };
        
        template <typename Tree>
        inline auto&& access(Tree&& tree) { // must be used internally or in tests.
            return tree.root_;
        }

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

    template <typename, typename>
    struct empty_augment {};

    template <typename Element, template <typename /*Element*/, typename /*Node*/> typename Augment, typename ... Capabilities>
    struct tree : public Capabilities... { 
        using value_type = Element;
        using node_type = aux::node<value_type, Augment>;
        using augment_type = typename node_type::augment_type;
        using ptr_type = typename node_type::ptr_type;

        aux::warning<Augment<value_type, node_type>, Capabilities...> warn_; // compile time warnings
    private:
        std::unique_ptr<node_type> root_; // no direct access. if anything, an access must be performed consiously via aux::access

        template <typename Tree>
        friend inline auto&& aux::access(Tree&& tree);
    };

    template <typename ... pack>
    size_t size(const tree<pack...>& tree) {
        return aux::size(aux::access(tree));
    }

    template <typename ... pack>
    size_t height(const tree<pack...>& tree) {
        return aux::height(aux::access(tree));
    }

    template <typename ... pack>
    bool balanced(const tree<pack...>& tree) {
        auto&& root = aux::access(tree);
        if (!root) return true;
        return std::abs(aux::height(root->left_) - aux::height(root->right_)) <= 1;
    }

    template <typename ... pack>
    bool perfect(const tree<pack...>& tree) {
        return aux::perfect(aux::access(tree)).first;
    }
}