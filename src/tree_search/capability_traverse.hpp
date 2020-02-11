#pragma once

#include "type_traverse.hpp"

#include <iterator>
#include <stack>

namespace tree_search {

    template <typename T, typename Node, typename TraverseTag>
    struct traverse_iterator;

    template <typename T, typename Node>
    struct traverse_iterator<T, Node, tag_preorder>
        : public std::iterator<std::forward_iterator_tag, T> {

        traverse_iterator() = default; // end iterator
        explicit traverse_iterator(Node* t) { this->s_.push(t); }
        traverse_iterator& operator++() { this->go(); return *this; }
        traverse_iterator operator++(int) { traverse_iterator retval = *this; ++(*this); return retval; }
        bool operator==(traverse_iterator other) const { return this->s_ == other.s_; }
        bool operator!=(traverse_iterator other) const { return !(*this == other); }
        reference operator*() const { return this->s_.top()->value_; }

    private:
        std::stack<Node*> s_ = {};

        void go() {
            if (this->s_.empty()) return;
            auto last = this->s_.top();
            this->s_.pop();
            if (last->right_) this->s_.push(last->right_.get());
            if (last->left_) this->s_.push(last->left_.get());
        }
    };

    template <typename T, typename Node>
    struct traverse_iterator<T, Node, tag_postorder>
        : public std::iterator<std::forward_iterator_tag, T> {

        traverse_iterator() = default; // end iterator
        explicit traverse_iterator(Node* t) { this->s_.emplace(cont{t}); this->go(); }
        traverse_iterator& operator++() { this->go(); return *this; }
        traverse_iterator operator++(int) { traverse_iterator retval = *this; ++(*this); return retval; }
        bool operator==(traverse_iterator other) const { return this->s_ == other.s_; }
        bool operator!=(traverse_iterator other) const { return !(*this == other); }
        reference operator*() const { return this->s_.top().ptr_->value_; }

    private:
        struct cont {
            Node* ptr_ = nullptr;
            volatile bool handled_ = false; // true if childrens were handled
            bool operator ==(const cont& other) const { return other.handled_ == this->handled_ && other.ptr_ == this->ptr_; }
        };

        std::stack<cont> s_ = {};

        void go() {
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

    template <typename T, typename Node>
    struct traverse_iterator<T, Node, tag_inorder>
        : public std::iterator<std::forward_iterator_tag, T> {

        traverse_iterator() = default; // end iterator
        explicit traverse_iterator(Node* t) { this->s_.push(t); }
        traverse_iterator& operator++() { this->go(); return *this; }
        traverse_iterator operator++(int) { traverse_iterator retval = *this; ++(*this); return retval; }
        bool operator==(traverse_iterator other) const { return this->s_ == other.s_; }
        bool operator!=(traverse_iterator other) const { return !(*this == other); }
        reference operator*() const { return this->s_.top()->value_; }

    private:
        std::stack<Node*> s_ = {};

        void go() {
            throw;
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
        using node_type = typename Tree::node_type;
        using value_type = typename Tree::value_type;
        using iterator_type = tree_search::traverse_iterator<value_type, node_type, Tag>;

        return iterators<iterator_type>{ iterator_type(tree.root_.get()) };
    }
}