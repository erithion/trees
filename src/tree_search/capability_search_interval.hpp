#pragma once 

#include "type_traverse.hpp"
#include "tree_interval.hpp"
#include "iterator_traverse.hpp"

#include <type_traits>
#include <utility>

namespace tree_search {

    namespace aux {

        template <typename Node>
        struct preorder_search {
            using cont = cont<Node>;
            using value_type = typename Node::value_type;
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
            std::stack<cont> stack_ = {};
            value_type       bounds_ = {};
        };

        template <typename Node>
        struct inorder_search {
            using cont = cont<Node>;
            using value_type = typename Node::value_type;
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
            std::stack<cont> stack_ = {};
            value_type       bounds_ = {};
        };

        template <typename Node>
        struct postorder_search {
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
        using selector_interval_search =
            std::conditional_t< std::is_same_v<Tag, tag_preorder>
                              , preorder_search<Node>
                              , std::conditional_t< std::is_same_v<Tag, tag_inorder>
                                                  , inorder_search<Node>
                                                  , std::conditional_t<std::is_same_v<Tag, tag_postorder>, postorder_search<Node>, std::void_t<>>
                                                  >
                              >;
    }

    template <typename Node, typename TraverseTag>
    struct search_iterator_interval
        : protected aux::selector_interval_search<Node, TraverseTag> { 

        using iterator_category = std::forward_iterator_tag;
        using value_type = typename Node::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        explicit search_iterator_interval(const value_type& interval) {
            this->bounds_ = interval;
        }

        search_iterator_interval(Node* root, const value_type& interval) {
            this->stack_.emplace(cont{ root });
            this->bounds_ = interval;
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
    };


    struct capability_search_interval {};

    namespace aux {

        template <typename Node>
        using callback = std::function<void(const typename Node::value_type&)>;

        template <typename T, typename Node>
        void search(const std::unique_ptr<Node>& tree, const std::pair<T, T>& interval, const callback<Node>& fn, tag_postorder&&) {
            if (!tree) return;
            if (tree->left_ && tree->left_->augment_.max_ >= interval.first)
                search(tree->left_, interval, fn, tag_inorder{});
            if (tree->value_.first <= interval.second)
                search(tree->right_, interval, fn, tag_inorder{});
            if (interval.first <= tree->value_.second && !(interval.second < tree->value_.first))
                if (fn) fn(tree->value_);
        }

        template <typename T, typename ... Ts>
        constexpr bool is_one_of_v = std::disjunction_v<std::is_same<T, Ts>...>;

        // Enables int type if interval tree requirements are satisfied
        template <typename Tag, typename T, typename Tree>
        using enable_interval_search_t =
            std::enable_if_t< std::is_base_of_v<capability_search_interval, Tree>
                           && aux::is_one_of_v<Tag, tag_inorder, tag_postorder, tag_preorder> // only for 3 predefined tags
                           && std::is_same_v<typename Tree::augment_type, aux::interval_augment<T>> // only for trees with interval_augment type within
            , int>;
    }

    // TODO: implement iterators for searching
    template <typename Tag, typename T, typename Tree, aux::enable_interval_search_t<Tag, T, Tree> = 0>
    auto search(Tag&& tag, const Tree& tree, const std::pair<T, T>& interval) {
       //return aux::search(tree.root_, interval, std::move(tag));
        using iterator_type = tree_search::search_iterator_interval<typename Tree::node_type, Tag>;
        return iterators<iterator_type>{ iterator_type(tree.root_.get(), interval), iterator_type(interval) };
    }
}