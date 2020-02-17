#pragma once

#include "type_traverse.hpp"
#include "iterator.hpp"

#include <stack>

namespace tree_search {

    template <typename Node, typename TraverseTag>
    struct iterator_traverse {

        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        iterator_traverse() = default; // end iterator

        // for preorder
        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_preorder>, int> = 0>
        explicit iterator_traverse(const Node* root) {
            this->stack_.push(root);
        }
        // for everything else but preorder
        template <typename U = TraverseTag, std::enable_if_t<!std::is_same_v<U, tag_preorder>, int> = 0>
        explicit iterator_traverse(const Node* root) {
            this->stack_.emplace(cont{ root });
            this->step();
        }
        iterator_traverse& operator++() {
            this->step(); 
            return *this; 
        }
        bool operator==(const iterator_traverse& other) const { 
            return this->stack_ == other.stack_;
        }
        //for preorder
        template <typename U = TraverseTag>
        std::enable_if_t<std::is_same_v<U, tag_preorder>, reference> operator*() const {
            return this->stack_.top()->value_;
        }
        // for everything else but preorder
        template <typename U = TraverseTag>
        std::enable_if_t<!std::is_same_v<U, tag_preorder>, reference> operator*() const {
            return this->stack_.top().ptr_->value_;
        }
    private:
        using cont = aux::cont<const Node>;
        using stack = std::conditional_t<std::is_same_v<TraverseTag, tag_preorder>, std::stack<const Node*>, std::stack<cont>>;

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_preorder>, int> = 0>
        void step() {
            if (this->stack_.empty()) return;
            auto last = this->stack_.top();
            this->stack_.pop();
            if (last->right_) this->stack_.push(last->right_.get());
            if (last->left_) this->stack_.push(last->left_.get());
        }

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_inorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            while (!this->stack_.empty() && !this->stack_.top().visited_) { // push children
                auto p = this->stack_.top().ptr_;
                this->stack_.pop();
                if (p->right_) this->stack_.emplace(cont{ p->right_.get() });
                this->stack_.emplace(cont{ p, true });
                if (p->left_) this->stack_.emplace(cont{ p->left_.get() });
            }
        }

        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<U, tag_postorder>, int> = 0>
        void step() {
            if (!this->stack_.empty() && this->stack_.top().visited_) this->stack_.pop();
            while (!this->stack_.empty() && !this->stack_.top().visited_) { // push children
                auto p = this->stack_.top().ptr_;
                this->stack_.top().visited_ = true;
                if (p->right_) this->stack_.emplace(cont{ p->right_.get() });
                if (p->left_) this->stack_.emplace(cont{ p->left_.get() });
            }
        }

        stack stack_ = {};
    };
}