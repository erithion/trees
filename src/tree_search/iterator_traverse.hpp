#pragma once

#include "type_traverse.hpp"
#include "iterator.hpp"

#include <stack>

namespace tree_search {

    // TODO: consider moving step into iterator
    namespace aux {

        template <typename Node>
        struct preorder {
            void step() {
                if (this->s_.empty()) return;
                auto last = this->s_.top();
                this->s_.pop();
                if (last->right_) this->s_.push(last->right_.get());
                if (last->left_) this->s_.push(last->left_.get());
            }
            std::stack<Node*> s_ = {};
        };

        template <typename Node>
        struct inorder {
            using cont = cont<Node>;
            void step() {
                if (this->s_.empty()) return;

                if (this->s_.top().visited_) this->s_.pop();
                if (!this->s_.empty() && !this->s_.top().visited_)
                    for (auto p = this->s_.top().ptr_; p; p = p->left_.get()) {
                        this->s_.pop();
                        if (p->right_) this->s_.emplace(cont{ p->right_.get() });
                        this->s_.emplace(cont{ p, true });
                        if (p->left_) this->s_.emplace(cont{ p->left_.get(), true });
                    }
            }
            std::stack<cont> s_ = {};
        };

        template <typename Node>
        struct postorder {
            using cont = cont<Node>;
            void step() {
                if (this->s_.empty()) return;

                if (this->s_.top().visited_) this->s_.pop();
                while (!this->s_.empty() && !this->s_.top().visited_) { // push children
                    auto& top = this->s_.top();
                    top.visited_ = true;
                    if (top.ptr_->right_) this->s_.emplace(cont{ top.ptr_->right_.get() });
                    if (top.ptr_->left_) this->s_.emplace(cont{ top.ptr_->left_.get() });
                }
            }
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
        : protected aux::selector<Node, TraverseTag> { // by TraverseTag choose the type from above to inherit from.
                                                       // so called "template method"-pattern has never been more justified. 
                                                       // otherwise there would have been 3 potentially error prone iterators
                                                       // where in one of them some alias/method might get lost one day during refactoring.  
        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        traverse_iterator() = default; // end iterator

        // for preorder
        template <typename U = TraverseTag, std::enable_if_t<std::is_same_v<aux::selector<Node, U>, aux::preorder<Node>>, int> = 0>
        explicit traverse_iterator(Node* root) {
            this->s_.push(root);
        }
        // for everything else but preorder
        template <typename U = TraverseTag, std::enable_if_t<!std::is_same_v<aux::selector<Node, U>, aux::preorder<Node>>, int> = 0>
        explicit traverse_iterator(Node* root) {
            this->s_.emplace(cont{ root });
            this->step();
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
        bool operator==(const traverse_iterator& other) const { 
            return this->s_ == other.s_;
        }
        bool operator!=(const traverse_iterator& other) const { 
            return !(*this == other); 
        }
        //for preorder
        template <typename U = TraverseTag>
        std::enable_if_t<std::is_same_v<aux::selector<Node, U>, aux::preorder<Node>>, reference>
            operator*() const {
            return this->s_.top()->value_;
        }
        // for everything else but preorder
        template <typename U = TraverseTag>
        std::enable_if_t<!std::is_same_v<aux::selector<Node, U>, aux::preorder<Node>>, reference>
            operator*() const {
            return this->s_.top().ptr_->value_;
        }
    };
}