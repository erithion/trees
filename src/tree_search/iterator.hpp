#pragma once
// Various iterator utils

namespace tree_search {

    namespace aux {

        // to ease tree traversing implementation.
        template <typename Node>
        struct cont {
            Node* ptr_ = nullptr;
            bool visited_ = false; // Signals if the current node's children have been already pushed on stack.
            bool operator ==(const cont& other) const { return other.visited_ == this->visited_ && other.ptr_ == this->ptr_; }
        };
    }

    template <typename It>
    struct iterators {
        It begin_ = {};
        It end_ = {};
    };

    // for range loop support.
    // absence of begin/end methods in a range class r leads to ADL of begin/end functions and ADL expects them to be here.
    template<typename It>
    It begin(const iterators<It>& r) {
        return r.begin_;
    }
    template<typename It>
    It end(const iterators<It>& r) {
        return r.end_;
    }

    // generic. eliminates the need to define a postfix ++ operator within an iterator.
    template <typename Iterator>
    Iterator operator++(Iterator& i, int) {
        Iterator retval = i;
        ++i;
        return retval;
    }
    // generic. eliminates the need to define != operator within an iterator.
    template <typename Iterator>
    bool operator!=(const Iterator& cur, const Iterator& other) {
        return !(cur == other);
    }
}