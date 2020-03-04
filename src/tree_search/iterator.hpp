#pragma once 

#include "type_traverse.hpp"
#include "iterator_aux.hpp"
#include "type_cti.hpp"
#include "tree.hpp"

#include <type_traits>
#include <stack>

namespace tree_search {

    namespace aux {

        // to ease tree traversing implementation.
        template <typename Tree>
        struct cont {
            const aux::node_type_t<Tree>* ptr_ = nullptr;
            bool                          visited_ = false; // Signals if the current node's children have been already pushed on stack.
            bool operator ==(const cont& other) const { return other.visited_ == this->visited_ && other.ptr_ == this->ptr_; }
        };

        template <typename Tree>
        struct universal_truth {
            inline bool operator()(const aux::node_type_t<Tree>*) const { return true; }
        };
    }

    template <typename Tree, typename TraverseTag
             , typename IntersectNode = aux::universal_truth<Tree>
             , typename IntersectLeftChild = aux::universal_truth<Tree>
             , typename IntersectRightChild = aux::universal_truth<Tree>>
    struct iterator {

        using iterator_category = std::forward_iterator_tag;
        using value_type = aux::value_type_t<Tree>;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        using node_type = aux::node_type_t<Tree>;
        iterator() = default;

        template <typename ... U>
        iterator(const node_type* root, const U&... u)
            : intersect_(u...), intersect_left_(u...), intersect_right_(u...) {
            this->stack_.emplace(cont{ root });
            this->step();
        }
        iterator& operator++() {
            this->step();
            return *this;
        }
        bool operator==(const iterator& other) const {
            return this->stack_ == other.stack_;
        }
        reference operator*() const {
            return this->stack_.top().ptr_->value_;
        }
    private:
        using intersect_left = IntersectLeftChild;
        using intersect_right = IntersectRightChild;
        using intersect = IntersectNode;
        using cont = aux::cont<node_type>;

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_preorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            // rearrangement on the stack is always in order (from the top): top; left; right.
            // upon loop exit there's the next node within the interval bounds on top of the stack.
            while (!this->stack_.empty() && !this->stack_.top().visited_) {
                auto p = this->stack_.top().ptr_;
                this->stack_.pop();
                if (p->right_ && this->intersect_right_(p))
                    this->stack_.emplace(cont{ p->right_.get() });
                if (p->left_ && this->intersect_left_(p))
                    this->stack_.emplace(cont{ p->left_.get() });
                if (p && this->intersect_(p))
                    this->stack_.emplace(cont{ p, true });
            }
        }

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_inorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            // rearrangement on the stack is always in order (from the top): left; top; right.
            // this has an effect as if we were always moving to the deepest left, then top, then right.
            // we skip the nodes beyond the interval's bound. 
            // upon loop exit there's the next node within the interval bounds on top of the stack.
            while (!this->stack_.empty() && !this->stack_.top().visited_) {
                auto p = this->stack_.top().ptr_;
                this->stack_.pop();
                if (p->right_ && this->intersect_right_(p))
                    this->stack_.emplace(cont{ p->right_.get() });
                if (p && this->intersect_(p))
                    this->stack_.emplace(cont{ p, true });
                if (p->left_ && this->intersect_left_(p))
                    this->stack_.emplace(cont{ p->left_.get() });
            }
        }

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_postorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            // rearrangement on the stack is always in order (from the top): left; right; top.
            while (!this->stack_.empty() && !this->stack_.top().visited_) {
                auto p = this->stack_.top().ptr_;
                this->stack_.pop();
                if (p && this->intersect_(p))
                    this->stack_.emplace(cont{ p, true });
                if (p->right_ && this->intersect_right_(p))
                    this->stack_.emplace(cont{ p->right_.get() });
                if (p->left_ && this->intersect_left_(p))
                    this->stack_.emplace(cont{ p->left_.get() });
            }
        }

        std::stack<cont> stack_ = {};
        intersect        intersect_ = {};
        intersect_left   intersect_left_ = {};
        intersect_right  intersect_right_ = {};
    };
}