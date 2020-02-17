#pragma once

namespace tree_search {

    namespace aux {

        template <typename Node>
        struct cont {
            Node* ptr_ = nullptr;
            bool visited_ = false; // Signals if the current node's children have been already pushed on stack
            bool operator ==(const cont& other) const { return other.visited_ == this->visited_ && other.ptr_ == this->ptr_; }
        };
    }

    template <typename It>
    struct iterators {
        It begin_;
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
}