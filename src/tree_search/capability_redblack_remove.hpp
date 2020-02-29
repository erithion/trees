/* Additional material
- https://www.youtube.com/watch?v=CTvfzU_uNKE deletion (Tushar Roy)
*/
#pragma once

#include "capability_rotate.hpp"
#include "type_traverse.hpp" // TODO: move is_one_of out
#include "iterator_aux.hpp" // TODO: move out node_type_t

#include <memory>
#include <algorithm>
#include <cassert>

namespace tree_search {

    namespace aux {

        template <typename Node>
        struct fix_remove_invariants {

            using ptr_t = typename Node::ptr_type;

            static bool left(ptr_t& parent, ptr_t& child) {
                return parent->left_ == child;
            }

            static bool right(ptr_t& parent, ptr_t& child) {
                return parent->right_ == child;
            }

            static inline bool black(ptr_t& p) {
                return !p /*sentinel node, hence black*/ || p->color_ == rb_black /*not sentinel and black*/;
            }

            static inline bool strong_black(ptr_t& p) {
                return p && p->color_ == rb_black;
            }

            static inline bool strong_red(ptr_t& p) {
                return p && p->color_ == rb_red;
            }

            static inline ptr_t* case3(ptr_t& parent, ptr_t& del) {
                // separate mirror cases. verbose yet better legible
                if (left(parent, del) 
                    && strong_black(parent)
                    && black(del)
                    && strong_black(parent->right_)
                    && black(parent->right_->left_)
                    && black(parent->right_->right_)) {

                    parent->right_->color_ = rb_red;
                    
                    return &parent;
                }
                else if (right(parent, del)
                    && strong_black(parent)
                    && black(del)
                    && strong_black(parent->left_)
                    && black(parent->left_->left_)
                    && black(parent->left_->right_)) {

                    parent->left_->color_ = rb_red;

                    return &parent;
                }
                return nullptr;
            }

            static inline ptr_t* case4(ptr_t& parent, ptr_t& del) {
                // separate mirror cases. verbose yet better legible
                if (left(parent, del) 
                    && strong_red(parent)
                    && black(del)
                    && strong_black(parent->right_)
                    && black(parent->right_->left_)
                    && black(parent->right_->right_)) {

                    std::swap(parent->color_, parent->right_->color_);
                } 
                else if (right(parent, del)
                    && strong_red(parent)
                    && black(del)
                    && strong_black(parent->left_)
                    && black(parent->left_->left_)
                    && black(parent->left_->right_)) {

                    std::swap(parent->color_, parent->left_->color_);
                }
                return nullptr;
            }

            static inline ptr_t* case5(ptr_t& parent, ptr_t& del) {
                // separate mirror cases. verbose yet better legible
                if (left(parent, del)
                    && strong_black(parent)
                    && black(del)
                    && strong_black(parent->right_)
                    && strong_red(parent->right_->left_)
                    && black(parent->right_->right_)) {

                    rotate(parent->right_, rotate_right_tag{});
                    std::swap(parent->right_->color_, parent->right_->right_->color_);
                }
                else if (right(parent, del)
                    && strong_black(parent)
                    && black(del)
                    && strong_black(parent->left_)
                    && strong_red(parent->left_->right_)
                    && black(parent->left_->right_)) {

                    rotate(parent->left_, rotate_left_tag{});
                    std::swap(parent->left_->color_, parent->left_->left_->color_);
                }
                return nullptr;
            }

            static inline ptr_t* case6(ptr_t& parent, ptr_t& del) {
                // separate mirror cases. verbose yet better legible
                if (left(parent, del) 
                    && black(del)
                    && strong_black(parent->right_)
                    && strong_red(parent->right_->right_)) {

                    std::swap(parent->color_, parent->right_->color_);
                    parent->right_->right_->color_ = rb_black;
                    rotate(parent, rotate_left_tag{});
                }
                else if (right(parent, del) 
                    && black(del)
                    && strong_black(parent->left_)
                    && strong_red(parent->left_->left_)) {

                    std::swap(parent->color_, parent->left_->color_);
                    parent->left_->left_->color_ = rb_black;
                    rotate(parent, rotate_right_tag{});
                }
                return nullptr;
            }
        };

        template <typename T, typename Node>
        std::unique_ptr<Node>* remove(std::unique_ptr<Node>& tree, const T& v, const capability_remove_redblack& tag) {
            using inv = fix_remove_invariants<Node>;

            inv::ptr_t* double_black = nullptr;
            if (!tree) return nullptr;
            else if (v < tree->value_) double_black = remove(tree->left_, v, tag);
            else if (v > tree->value_) double_black = remove(tree->right_, v, tag);
            else { // found
                if (tree->left_ && tree->right_) {
                    auto p = std::ref(tree->right_);
                    for (p = tree->right_; p.get()->left_; p = p.get()->left_);
                    std::swap(tree->value_, p.get()->value_);
                    double_black = remove(tree->right_, v, tag);
                }
                else if (tree->left_) {
                    std::swap(tree->value_, tree->left_->value_);
                    double_black = remove(tree->left_, v, tag);
                }
                else if (tree->right_) {
                    std::swap(tree->value_, tree->right_->value_);
                    double_black = remove(tree->right_, v, tag);
                }
                else {
                    auto color = tree->color_;
                    tree = nullptr; // removing the leaf
                    return color == rb_black ? &tree : nullptr; // returning a double black leaf if we have just removed black leaf
                }
            }
            if (double_black != nullptr) {
                auto& other = tree->left_ == *double_black ? tree->right_ : tree->left_;
                auto p = inv::case3(tree, *double_black);
                if (p != nullptr) {
                    return p;
                }
                inv::case4(tree, *double_black);
                inv::case5(tree, *double_black);
                inv::case6(tree, *double_black);
            }
            return nullptr;
        }

        // Enables int type if red-black tree requirements are satisfied
        template <typename T, typename Tree>
        using enable_redblack_remove_t =
            std::enable_if_t< std::is_base_of_v<capability_remove_redblack, Tree> // only for capability_remove_redblack
                            && std::is_same_v<typename Tree::augment_type, aux::redblack_augment<std::decay_t<T>, aux::node_type_t<Tree>>> // only for redblack_augment
                            && !std::is_base_of_v<capability_rotate, Tree> // rotation available on a user side may invalidate invariants
            , int>;
    }

    template <typename T, typename Tree, aux::enable_redblack_remove_t<T, Tree> = 0>
    void remove(Tree& tree, const T& v) {
        aux::remove(aux::access(tree), v, capability_remove_redblack{});
        aux::access(tree)->color_ = aux::rb_black;
    }

    template <typename T, typename Tree, aux::enable_redblack_remove_t<T, Tree> = 0>
    void remove(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) remove(tree, v);
    }
}