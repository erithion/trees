#pragma once 

#include "type_traverse.hpp"
#include "iterator.hpp"
#include "tree_interval.hpp"

#include <type_traits>
#include <stack>

namespace tree_search {

    template <typename Node, typename TraverseTag>
    struct iterator_search_interval {

        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        explicit iterator_search_interval(const value_type& interval)
            : bounds_(interval) {}

        iterator_search_interval(const Node* root, const value_type& interval) 
            : bounds_(interval) {
            this->stack_.emplace(cont{ root });
            this->step();
        }
        iterator_search_interval& operator++() {
            this->step();
            return *this;
        }
        bool operator==(const iterator_search_interval& other) const {
            return this->stack_ == other.stack_ && this->bounds_ == other.bounds_;
        }
        reference operator*() const {
            return this->stack_.top().ptr_->value_;
        }
    private:
        using cont = aux::cont<const Node>;

        inline bool intersect_right(const Node* p) const { // right nodes' left border gets ever bigger with each next step down the tree 
            return p->right_ && p->right_->value_.first <= this->bounds_.second;
        }

        inline bool intersect_left(const Node* p) const { // left nodes' max border MIGHT get smaller with each next step down the tree 
            return p->left_ && p->left_->augment_.max_ >= this->bounds_.first;
        }

        inline bool intersect(const Node* p) const { // covers all 3 possible cases of intersection with the node
            return this->bounds_.first <= p->value_.second && this->bounds_.second >= p->value_.first;
        }

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_preorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            // rearrangement on the stack is always in order (from the top): top; left; right.
            // upon loop exit there's the next node within the interval bounds on top of the stack.
            while (!this->stack_.empty() && !this->stack_.top().visited_) {
                auto p = this->stack_.top().ptr_;
                this->stack_.pop();
                if (this->intersect_right(p))
                    this->stack_.emplace(cont{ p->right_.get() });
                if (this->intersect_left(p))
                    this->stack_.emplace(cont{ p->left_.get() });
                if (this->intersect(p))
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
                if (this->intersect_right(p))
                    this->stack_.emplace(cont{ p->right_.get() });
                if (this->intersect(p))
                    this->stack_.emplace(cont{ p, true });
                if (this->intersect_left(p))
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
                if (this->intersect(p))
                    this->stack_.emplace(cont{ p, true });
                if (this->intersect_right(p))
                    this->stack_.emplace(cont{ p->right_.get() });
                if (this->intersect_left(p))
                    this->stack_.emplace(cont{ p->left_.get() });
            }
        }

        std::stack<cont> stack_ = {};
        value_type       bounds_ = {};
    };
}