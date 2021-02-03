#pragma once
// Various iterator utils

namespace tree_search {

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
    template <typename It>
    It operator++(It& i, int) {
        It retval = i;
        ++i;
        return retval;
    }
    // generic. eliminates the need to define != operator within an iterator.
    template <typename It>
    bool operator!=(const It& cur, const It& other) {
        return !(cur == other);
    }
}