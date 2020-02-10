#pragma once

#include "type_traverse.hpp"

#include <functional>
#include <type_traits>
#include <memory>
#include <iterator>
#include <stack>

namespace tree_search {

    template <typename T, typename Node, typename TraverseTag>
    struct iterator_;

    template <typename T, typename Node>
    struct iterator_<T, Node, tag_preorder>
        : public std::iterator<std::forward_iterator_tag, T> {

        explicit  iterator_(Node* t) : s_() { this->s_.push(t); }
        iterator_& operator++() { this->go(); return *this; }
        iterator_ operator++(int) { iterator_ retval = *this; ++(*this); return retval; }
        bool operator==(iterator_ other) const { return this->s_ == other.s_; }
        bool operator!=(iterator_ other) const { return !(*this == other); }
        reference operator*() const { return this->s_.top()->value_; }

    private:
        std::stack<Node*> s_;

        void go() {
            if (this->s_.empty()) return;
            auto last = this->s_.top();
            this->s_.pop();
            if (last->right_) this->s_.push(last->right_.get());
            if (last->left_) this->s_.push(last->left_.get());
        }
    };
    template <typename T, typename Node>
    struct iterator_<T, Node, tag_postorder>
        : public std::iterator<std::forward_iterator_tag, T> {

        explicit  iterator_(Node* t) { this->s_.push(std::make_pair(t, false)); this->go(); }
        iterator_& operator++() { this->go(); return *this; }
        iterator_ operator++(int) { iterator_ retval = *this; ++(*this); return retval; }
        bool operator==(iterator_ other) const { return this->s_ == other.s_; }
        bool operator!=(iterator_ other) const { return !(*this == other); }
        reference operator*() const { return this->s_.top().first->value_; }

    private:
        std::stack<std::pair<Node*, volatile bool>> s_ = {};

        void put_children() {
            auto& top = this->s_.top();
            if (!top.second) {
                top.second = true;
                if (top.first->right_) this->s_.emplace(std::make_pair(top.first->right_.get(), false));
                if (top.first->left_) this->s_.emplace(std::make_pair(top.first->left_.get(), false));
            }
        }
        void go() {
            if (this->s_.empty()) return;

            if (this->s_.top().second) this->s_.pop();
            while (!this->s_.top().second) this->put_children();
        }
    };

    //template <typename T, typename Tree>
    //struct iterator<T, Tree, tag_inorder>
    //    : public std::iterator<T, std::forward_iterator_tag> {

    //    explicit  iterator(const Tree* t)
    //        : s_(t), f_(false) {}

    //    iterator& operator++() { num = TO >= FROM ? num + 1 : num - 1; return *this; }
    //    iterator operator++(int) { iterator retval = *this; ++(*this); return retval; }
    //    bool operator==(iterator other) const { return num == other.num; }
    //    bool operator!=(iterator other) const { return !(*this == other); }
    //    reference operator*() const { return this->s_.top()->value_; }

    //private:
    //    std::stack<const Tree*> s_;
    //    bool f_;

    //    void go() {
    //        while (!this->f_ && this->s_.top()->left_) {
    //            this->s_.push(this->s_.top()->left_);
    //            return;
    //        }
    //        if (!this->f_) {
    //            auto last = this->s_.top();
    //            this->s_.pop();
    //            if (last->right_) {
    //                this->s_.push(last->right_);
    //                this->go();
    //            } else 
    //                this->f_ = true;
    //            return;
    //        }
    //        this->f_ = false;
    //        if (this->s_.top()->right) {
    //            this->s_.push(this->s_.top()->right);
    //            this->go();
    //        }
    //    }
    //};
  

    struct capability_traverse {};

    namespace aux {

        template <typename Tree, typename Tag>
        auto traverse(const std::unique_ptr<Tree>& tree, Tag&&) {
            return tree_search::iterator_<typename Tree::value_type, Tree, Tag>(tree.get());
        }
        //template <typename Tree>
        //void traverse(const std::unique_ptr<Tree>& tree, const std::function<void(const typename Tree::value_type&)>& fn, tag_inorder&&) {
        //    if (!tree) return;
        //    traverse(tree->left_, fn, tag_inorder{});
        //    if (fn) fn(tree->value_);
        //    traverse(tree->right_, fn, tag_inorder{});
        //}

        //template <typename Tree>
        //void traverse(const std::unique_ptr<Tree>& tree, const std::function<void(const typename Tree::value_type)>& fn, tag_preorder&&) {
        //    if (!tree) return;
        //    if (fn) fn(tree->value_);
        //    traverse(tree->left_, fn, tag_preorder{});
        //    traverse(tree->right_, fn, tag_preorder{});
        //}

        //template <typename Tree>
        //void traverse(const std::unique_ptr<Tree>& tree, const std::function<void(const typename Tree::value_type&)>& fn, tag_postorder&&) {
        //    if (!tree) return;
        //    traverse(tree->left_, fn, tag_postorder{});
        //    traverse(tree->right_, fn, tag_postorder{});
        //    if (fn) fn(tree->value_);
        //}
    }

    // TODO: implement iterators for traversing
    template <typename Tag, typename Tree, std::enable_if_t<std::is_base_of_v<capability_traverse, Tree>, int> = 0>
    auto traverse(Tag&& tag, const Tree& tree) {
        return aux::traverse(tree.root_, std::forward<Tag>(tag));
    }
}