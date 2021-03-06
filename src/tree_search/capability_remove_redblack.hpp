/* Additional material
- https://www.youtube.com/watch?v=CTvfzU_uNKE deletion (Tushar Roy)
- https://en.wikipedia.org/wiki/Red–black_tree#Removal consulted 

- http://staff.ustc.edu.cn/~csli/graduate/algorithms/book6/chap14.htm somewhat different and interesting deletion. requires parent though
*/
#pragma once

#include "capability_rotate.hpp"
#include "type_cti.hpp"

#include <memory> // std::unique_ptr

namespace tree_search {

    namespace aux {

        template <typename Node>
        inline void fixup(std::unique_ptr<Node>& tree, capability_remove_redblack) {} // in case a descendant tree needs it

        template <typename Node>
        struct rb_invariants {

            using ptr_t = typename Node::ptr_type;

            static inline ptr_t* fix(ptr_t& tree, ptr_t& del) { // returns a new double black or nullptr
                auto m = case2(tree, del);
                if (m != nullptr // the parent has been moved down to m during rotation. so we handle it.
                    && fix(*m, del) == nullptr) return nullptr; // if no double blacks have come up from below, then we're done.
                                                                 // otherwise a new double black must be in the 'tree'-var, so continuing.

                auto p = case3(tree, del);
                if (p != nullptr) return p; // double black has moved up one level
                if (!case4(tree, del)) {
                    case5(tree, del);
                    case6(tree, del);
                }
                return nullptr;
            }

        private:
            static inline ptr_t& sibling(ptr_t& parent, ptr_t& child) {
                return parent->left_ == child ? parent->right_ : parent->left_;
            }

            static inline bool left(ptr_t& parent, ptr_t& child) {
                return parent->left_ == child;
            }

            static inline bool right(ptr_t& parent, ptr_t& child) {
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

            static inline ptr_t* case2(ptr_t& parent, ptr_t& del) { // returns new parent
                // left and right cases are covered
                auto& sbl = sibling(parent, del);
                if (strong_red(sbl) 
                    && black(del)
                    && strong_black(parent)
                    && black(sbl->right_)
                    && black(sbl->left_)) {

                    std::swap(parent->color_, sbl->color_);
                    if (left(parent, del)) {
                        rotate(parent, rotate_left_tag{});
                        return &parent->left_; // parent has changed after rotation. returning to where it was
                    }
                    else if (right(parent, del)) {
                        rotate(parent, rotate_right_tag{});
                        return &parent->right_; // parent has changed after rotation. returning to where it was
                    }
                }
                // proceed to case 3 unconditionaly
                return nullptr;
            }

            static inline ptr_t* case3(ptr_t& parent, ptr_t& del) { // returns new double black
                // covers both mirrored cases
                auto& sbl = sibling(parent, del);
                if (strong_black(parent)
                    && black(del)
                    && strong_black(sbl)
                    && black(sbl->left_)
                    && black(sbl->right_)) {

                    sbl->color_ = rb_red;
                    // restart from case 1 with parent
                    return &parent;
                }
                // proceed to case 4
                return nullptr;
            }

            static inline bool case4(ptr_t& parent, ptr_t& del) {
                // covers both mirror cases because only parent is red here
                auto& sbl = sibling(parent, del);
                if (strong_red(parent)
                    && black(del)
                    && strong_black(sbl)
                    && black(sbl->left_)
                    && black(sbl->right_)) {

                    std::swap(parent->color_, sbl->color_);
                    return true; // terminal
                }
                return false; // proceed to case 5
            }

            static inline void case5(ptr_t& parent, ptr_t& del) {
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
                    && black(parent->left_->left_)) {

                    rotate(parent->left_, rotate_left_tag{});
                    std::swap(parent->left_->color_, parent->left_->left_->color_);
                }
                // proceed to 6 unconditionally
            }

            static inline void case6(ptr_t& parent, ptr_t& del) {
                // separate mirror cases. verbose yet better legible
                if (left(parent, del)
                    && black(del)
                    && strong_black(parent->right_)
                    && strong_red(parent->right_->right_)) {

                    parent->color_ = std::move(parent->right_->color_);
                    parent->right_->right_->color_ = rb_black;
                    rotate(parent, rotate_left_tag{});
                }
                else if (right(parent, del)
                    && black(del)
                    && strong_black(parent->left_)
                    && strong_red(parent->left_->left_)) {

                    parent->color_ = std::move(parent->left_->color_);
                    parent->left_->left_->color_ = rb_black;
                    rotate(parent, rotate_right_tag{});
                }
            }
        };

        template <typename T, typename Node, typename Tag>
        std::unique_ptr<Node>* rbremove(std::unique_ptr<Node>& tree, const T& v, Tag tag) {

            rb_invariants<Node>::ptr_t* double_black_child = nullptr;
            if (!tree) return nullptr;
            else if (v < tree->value_) double_black_child = rbremove(tree->left_, v, tag);
            else if (v > tree->value_) double_black_child = rbremove(tree->right_, v, tag);
            else { // found

                if (tree->left_ && tree->right_) {
                    auto ptr = tree->right_.get();
                    while (ptr->left_) ptr = ptr->left_.get();
                    std::swap(tree->value_, ptr->value_);
                    double_black_child = rbremove(tree->right_, v, tag);
                }
                else if (tree->left_) { // simple case with replacement
                    std::swap(tree->value_, tree->left_->value_);
                    double_black_child = rbremove(tree->left_, v, tag);
                }
                else if (tree->right_) { // simple case with replacement
                    std::swap(tree->value_, tree->right_->value_);
                    double_black_child = rbremove(tree->right_, v, tag);
                }
                else { // found a leaf. deleting
                    auto color = tree->color_;
                    tree = nullptr; // removing the leaf
                    return color == rb_black ? &tree : nullptr; // signaling double black if we have just removed a black leaf
                }
            }
            auto r = double_black_child != nullptr
                ? rb_invariants<Node>::fix(tree, *double_black_child) // handling double black node cases
                : nullptr;
            fixup(tree, tag);
            return r;
        }

        // Enables int type if red-black tree requirements are satisfied
        template <typename T, typename Tree, typename U = std::decay_t<T>>
        using enable_redblack_remove_t =
            std::enable_if_t< std::is_base_of_v<capability_remove_redblack, Tree> // only for capability_remove_redblack
                            && std::is_same_v<augment_type_t<Tree>, redblack_augment<U, node_type_t<Tree>>> // only for redblack_augment
                            && !std::is_base_of_v<capability_rotate, Tree> // rotation available on a user side may invalidate invariants
            , int>;
    }

    template <typename T, typename Tree, aux::enable_redblack_remove_t<T, Tree> = 0>
    void remove(Tree& tree, const T& v) {
        aux::rbremove(aux::access(tree), v, capability_remove_redblack{});
        aux::access(tree)->color_ = rb_black;
    }

    template <typename T, typename Tree, aux::enable_redblack_remove_t<T, Tree> = 0>
    void remove(Tree& tree, std::initializer_list<T> ls) {
        for (auto&& v : ls) remove(tree, v);
    }

    template <typename It, typename Tree, std::enable_if_t<std::is_base_of_v<capability_remove_redblack, Tree>, int> = 0>
    void remove(Tree& tree, It begin, It end) {
        while (begin != end) remove(tree, *begin++);
    }
}