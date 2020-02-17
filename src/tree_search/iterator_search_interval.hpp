#pragma once 

#include "type_traverse.hpp"
#include "iterator.hpp"
#include "tree_interval.hpp"

#include <type_traits>
#include <stack>

namespace tree_search {

    template <typename Node, typename TraverseTag>
    struct search_iterator_interval {

        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        explicit search_iterator_interval(const value_type& interval)
            : bounds_(interval) {}

        search_iterator_interval(Node* root, const value_type& interval) 
            : bounds_(interval) {
            this->stack_.emplace(cont{ root });
            this->step();
        }
        search_iterator_interval& operator++() {
            this->step();
            return *this;
        }
        search_iterator_interval operator++(int) {
            search_iterator_interval retval = *this;
            ++(*this);
            return retval;
        }
        bool operator==(const search_iterator_interval& other) const {
            return this->stack_ == other.stack_ && this->bounds_ == other.bounds_;
        }
        bool operator!=(const search_iterator_interval& other) const {
            return !(*this == other);
        }
        reference operator*() const {
            return this->stack_.top().ptr_->value_;
        }
    private:
        using cont = aux::cont<Node>;

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_preorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            // nodes on the stack are always rearranged in order (from the top): top; left; right.
            // upon the loop exit we always have the next highest node within the interval bounds.
            while (!this->stack_.empty() && !this->stack_.top().visited_) {
                auto p = this->stack_.top().ptr_;
                this->stack_.pop();
                if (p->right_ && p->right_->value_.first <= this->bounds_.second)
                    this->stack_.emplace(cont{ p->right_.get() });
                if (p->left_ && p->left_->augment_.max_ >= this->bounds_.first)
                    this->stack_.emplace(cont{ p->left_.get() });
                if (this->bounds_.first <= p->value_.second && this->bounds_.second >= p->value_.first)
                    this->stack_.emplace(cont{ p, true });
            }
        }

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_inorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            // nodes on the stack are always rearranged in order (from the top): left; top; right.
            // this has an effect as if we were always moving first to the left, then top, then right.
            // we skip the nodes beyond the interval's bound. 
            // upon the loop exit we always have the next lowest node within the interval bounds.
            while (!this->stack_.empty() && !this->stack_.top().visited_) {
                auto p = this->stack_.top().ptr_;
                this->stack_.pop();
                if (p->right_ && p->right_->value_.first <= this->bounds_.second)
                    this->stack_.emplace(cont{ p->right_.get() });
                if (this->bounds_.first <= p->value_.second && this->bounds_.second >= p->value_.first)
                    this->stack_.emplace(cont{ p, true });
                if (p->left_ && p->left_->augment_.max_ >= this->bounds_.first)
                    this->stack_.emplace(cont{ p->left_.get() });
            }
        }

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_postorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            // nodes on the stack are always rearranged in order (from the top): left; right; top.
            while (!this->stack_.empty() && !this->stack_.top().visited_) {
                auto p = this->stack_.top().ptr_;
                if (this->bounds_.first <= p->value_.second && this->bounds_.second >= p->value_.first)
                    this->stack_.top().visited_ = true;
                if (p->right_ && p->right_->value_.first <= this->bounds_.second)
                    this->stack_.emplace(cont{ p->right_.get() });
                if (p->left_ && p->left_->augment_.max_ >= this->bounds_.first)
                    this->stack_.emplace(cont{ p->left_.get() });
            }
        }

        std::stack<cont> stack_ = {};
        value_type       bounds_ = {};
    };
}