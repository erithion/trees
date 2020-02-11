#pragma once

#include "type_traverse.hpp"

#include <stack>

namespace tree_search {

    template <typename Node, typename TraverseTag>
    struct traverse_iterator;

    template <typename Node>
    struct traverse_iterator<Node, tag_preorder> {

        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        traverse_iterator() = default; // end iterator
        explicit traverse_iterator(Node* t) { this->s_.push(t); }
        traverse_iterator& operator++() { this->step(); return *this; }
        traverse_iterator operator++(int) { traverse_iterator retval = *this; ++(*this); return retval; }
        bool operator==(traverse_iterator other) const { return this->s_ == other.s_; }
        bool operator!=(traverse_iterator other) const { return !(*this == other); }
        reference operator*() const { return this->s_.top()->value_; }

    private:
        std::stack<Node*> s_ = {};

        void step() {
            if (this->s_.empty()) return;
            auto last = this->s_.top();
            this->s_.pop();
            if (last->right_) this->s_.push(last->right_.get());
            if (last->left_) this->s_.push(last->left_.get());
        }
    };

    template <typename Node>
    struct traverse_iterator<Node, tag_postorder> {

        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        traverse_iterator() = default; // end iterator
        explicit traverse_iterator(Node* t) { this->s_.emplace(cont{t}); this->step(); }
        traverse_iterator& operator++() { this->step(); return *this; }
        traverse_iterator operator++(int) { traverse_iterator retval = *this; ++(*this); return retval; }
        bool operator==(traverse_iterator other) const { return this->s_ == other.s_; }
        bool operator!=(traverse_iterator other) const { return !(*this == other); }
        reference operator*() const { return this->s_.top().ptr_->value_; }

    private:
        struct cont {
            Node* ptr_ = nullptr;
            volatile bool handled_ = false; // true if children were handled
            bool operator ==(const cont& other) const { return other.handled_ == this->handled_ && other.ptr_ == this->ptr_; }
        };
        std::stack<cont> s_ = {};

        void step() {
            if (this->s_.empty()) return;

            if (this->s_.top().handled_) this->s_.pop();
            while (!this->s_.empty() && !this->s_.top().handled_) { // push children
                auto& top = this->s_.top();
                top.handled_ = true;
                if (top.ptr_->right_) this->s_.emplace(cont{ top.ptr_->right_.get() });
                if (top.ptr_->left_) this->s_.emplace(cont{ top.ptr_->left_.get() });
            }
        }
    };

    template <typename Node>
    struct traverse_iterator<Node, tag_inorder> {

        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        traverse_iterator() = default; // end iterator
        explicit traverse_iterator(Node* t) { this->s_.emplace(cont{ t }); this->step(); }
        traverse_iterator& operator++() { this->step(); return *this; }
        traverse_iterator operator++(int) { traverse_iterator retval = *this; ++(*this); return retval; }
        bool operator==(traverse_iterator other) const { return this->s_ == other.s_; }
        bool operator!=(traverse_iterator other) const { return !(*this == other); }
        reference operator*() const { return this->s_.top().ptr_->value_; }

    private:
        struct cont {
            Node* ptr_ = nullptr;
            bool exit_ = false;
            bool operator ==(const cont& other) const { return other.exit_ == this->exit_ && other.ptr_ == this->ptr_; }
        };
        std::stack<cont> s_ = {};

        void step() {
            if (this->s_.empty()) return;

            if (this->s_.top().exit_) this->s_.pop();
            if (!this->s_.empty() && !this->s_.top().exit_) 
                for (auto p = this->s_.top().ptr_; p; p = p->left_.get()) {
                    this->s_.pop();
                    if (p->right_) this->s_.emplace(cont{ p->right_.get() });
                    this->s_.emplace(cont{ p, true });
                    if (p->left_) this->s_.emplace(cont{ p->left_.get(), true });
                }
        }
    };
  
    template <typename It>
    struct iterators {
        It begin;
        It end = {};
    };

    struct capability_traverse {};

    template <typename Tag, typename Tree, std::enable_if_t<std::is_base_of_v<capability_traverse, Tree>, int> = 0>
    auto traverse(Tag&& tag, const Tree& tree) {
        using iterator_type = tree_search::traverse_iterator<typename Tree::node_type, Tag>;
        return iterators<iterator_type>{ iterator_type(tree.root_.get()) };
    }
}