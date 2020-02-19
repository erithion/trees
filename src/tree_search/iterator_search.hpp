#pragma once 

#include "type_traverse.hpp"
#include "iterator.hpp"
#include "tree.hpp"

#include <type_traits>
#include <stack>

namespace tree_search {

    namespace aux {
        template <typename Node>
        constexpr auto universal_truth = [](const Node*) { return true; };
    }

    template <typename Node, typename TraverseTag>
    struct iterator_search {

        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        using condition = std::function<bool(const Node*)>;

        iterator_search() = default;

        iterator_search(const Node* root, condition&& here, condition&& to_left = aux::universal_truth<Node>, condition&& to_right = aux::universal_truth<Node>)
            : here_(std::move(here)), to_left_(std::move(to_left)), to_right_(std::move(to_right))
        {
            this->stack_.emplace(cont{ root });
            this->step();
        }
        iterator_search& operator++() {
            this->step();
            return *this;
        }
        bool operator==(const iterator_search& other) const {
            return this->stack_ == other.stack_;
        }
        reference operator*() const {
            return this->stack_.top().ptr_->value_;
        }
    private:
        using cont = aux::cont<const Node>;

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_preorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            // rearrangement on the stack is always in order (from the top): top; left; right.
            // upon loop exit there's the next node within the interval bounds on top of the stack.
            while (!this->stack_.empty() && !this->stack_.top().visited_) {
                auto p = this->stack_.top().ptr_;
                this->stack_.pop();
                if (p->right_ && this->to_right_(p))
                    this->stack_.emplace(cont{ p->right_.get() });
                if (p->left_ && this->to_left_(p))
                    this->stack_.emplace(cont{ p->left_.get() });
                if (p && this->here_(p))
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
                if (p->right_ && this->to_right_(p))
                    this->stack_.emplace(cont{ p->right_.get() });
                if (p && this->here_(p))
                    this->stack_.emplace(cont{ p, true });
                if (p->left_ && this->to_left_(p))
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
                if (p && this->here_(p))
                    this->stack_.emplace(cont{ p, true });
                if (p->right_ && this->to_right_(p))
                    this->stack_.emplace(cont{ p->right_.get() });
                if (p->left_ && this->to_left_(p))
                    this->stack_.emplace(cont{ p->left_.get() });
            }
        }

        std::stack<cont> stack_ = {};
        condition        here_ = {};
        condition        to_left_ = {};
        condition        to_right_ = {};
    };
}