#pragma once

#include "type_traverse.hpp"

#include <stack>

namespace tree_search {

    namespace aux {

        template <typename Node>
        struct preorder {
            void init(Node* root) {
                this->s_.push(root);
            }
            void step() {
                if (this->s_.empty()) return;
                auto last = this->s_.top();
                this->s_.pop();
                if (last->right_) this->s_.push(last->right_.get());
                if (last->left_) this->s_.push(last->left_.get());
            }
            bool equal(const preorder& other) const { return this->s_ == other.s_; }
            const typename Node::value_type& top_value() const { return this->s_.top()->value_; }
            std::stack<Node*> s_ = {};
        };

        template <typename Node>
        struct inorder {
            void init(Node* root) {
                this->s_.emplace(cont{ root });
                this->step();
            }
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
            bool equal(const inorder& other) const { return this->s_ == other.s_; }
            const typename Node::value_type& top_value() const { return this->s_.top().ptr_->value_; }
            struct cont {
                Node* ptr_ = nullptr;
                bool exit_ = false;
                bool operator ==(const cont& other) const { return other.exit_ == this->exit_ && other.ptr_ == this->ptr_; }
            };
            std::stack<cont> s_ = {};
        };

        template <typename Node>
        struct postorder {
            void init(Node* root) {
                this->s_.emplace(cont{ root });
                this->step();
            }
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
            bool equal(const postorder& other) const { return this->s_ == other.s_; }
            const typename Node::value_type& top_value() const { return this->s_.top().ptr_->value_; }
            struct cont {
                Node* ptr_ = nullptr;
                volatile bool handled_ = false; // true if children were handled
                bool operator ==(const cont& other) const { return other.handled_ == this->handled_ && other.ptr_ == this->ptr_; }
            };
            std::stack<cont> s_ = {};
        };

        // Chooses the appropriate preorder/inorder/postorder type in response to Tag
        template <typename Node, typename Tag>
        using selector =
            std::conditional_t< std::is_same_v<Tag, tag_preorder>
                              , preorder<Node>
                              , std::conditional_t< std::is_same_v<Tag, tag_inorder>
                                                  , inorder<Node>
                                                  , std::conditional_t<std::is_same_v<Tag, tag_postorder>, postorder<Node>, std::void_t<>>
                                                  >
                              >;
    }

    template <typename Node, typename TraverseTag>
    struct traverse_iterator
        : protected aux::selector<Node, TraverseTag> { // by TraverseTag choose an appropriate type from above to inherit from.
                                                       // so called "template method"-pattern is entirely justified here. 
                                                       // otherwise there would have been 3 potentially error prone iterators
                                                       // where in one of them during refactoring one may easily loose 
                                                       // some required alias or method.  
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        traverse_iterator() = default; // end iterator
        explicit traverse_iterator(Node* root) { 
            this->init(root); 
        }
        traverse_iterator& operator++() { 
            this->step(); 
            return *this; 
        }
        traverse_iterator operator++(int) { 
            traverse_iterator retval = *this; 
            ++(*this); 
            return retval; 
        }
        bool operator==(traverse_iterator other) const { 
            return this->equal(other); 
        }
        bool operator!=(traverse_iterator other) const { 
            return !(*this == other); 
        }
        reference operator*() const { 
            return this->top_value(); 
        }
    };
}