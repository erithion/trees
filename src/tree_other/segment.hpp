/* Additional material
   - https://cp-algorithms.com/data_structures/segment_tree.html
   - https://www.hackerearth.com/practice/data-structures/advanced-data-structures/segment-trees/tutorial/
*/
#pragma once

#include <vector>
#include <initializer_list>
#include <iterator>

namespace tree_other {

    template <typename T, typename Functional>
    struct segment {
        using result_type = typename Functional::result_type;
        using functional_type = Functional;

        // TODO: various lvalue/rvalue constructors
        segment(std::initializer_list<T>&& ls)
            : vec_(tree_size(ls.size())) {
            compute(this->vec_, 0, ls.begin(), ls.end());
        }

        auto view() const {
            return this->vec_;
        }

    private:

        template <typename T, typename F>
        friend auto query(const segment<T, F>& s, const std::pair<size_t, size_t>& bounds);

        template <typename T, typename F>
        friend auto update(segment<T, F>& s, const std::pair<size_t, size_t>& bounds, T&& val);

        template <typename T, typename F>
        friend auto update(segment<T, F>& s, const std::pair<size_t, size_t>& bounds, const T& val);

        constexpr static size_t tree_size(size_t n) {
            // Height of a binary tree required to fit all n elements: H = ceil [ log2 n ];
            // Hence the number of nodes to fit all n elements in the leafs: 
            //    S = 1 + 2 + 4 + ... + 2 ^ H = { using geometric progression: (a^0-a^(n+1))/(1-a) } = 2^(H+1) - 1 
            // Since 2 ^ (log2 n) = n except for 'ceil' function, we can get rid of calculating log2: 
            //    S = 2 ^ (H + 1) - 1 = 2 * 2 ^ H - 1 = 2 * 2 ^ (ceil [ log2 n ]) - 1 < 2 * 2 * 2 ^ log2 n = 4 * N => S < 4 * N
            return 4 * n;
        }

        template <typename It>
        static void compute(std::vector<T>& out, size_t idx, It begin, It end) {
            auto d = std::distance(begin, end);
            if (d == 1) out[idx] = *begin;
            else if (d > 1) {
                size_t m = d / 2;
                compute(out, idx * 2 + 1, begin, begin + m);
                compute(out, idx * 2 + 2, begin + m, end);
                out[idx] = functional_type()(out[idx * 2 + 1], out[idx * 2 + 2]);
            }
        }

        // verbose algorithm.
        // neater version with out-of-bounds check would require introducing the identity math concept w.r.t. used operation
        // which is not always possible. examples without identity are xor or modulus operations.
        // this version is just a little wordy, yet works without the need for identities.
        auto query(size_t loc, std::pair<size_t, size_t>&& cur, std::pair<size_t, size_t>&& in) const {
            if (cur.first == in.first && in.second + 1 == cur.second) return this->vec_[loc]; // have reached a leaf
            size_t m = (cur.second - cur.first) / 2;
            auto left = std::make_pair(cur.first, cur.first + m);
            auto right = std::make_pair(cur.first + m, cur.second);
            if (in.first >= left.first && in.second < left.second) return query(2 * loc + 1, std::move(left), std::move(in)); // if completely within the left subtree
            else if (in.first >= right.first && in.second < right.second) return query(2 * loc + 2, std::move(right), std::move(in)); // if completely within the right subtree
            else 
                return functional_type()( query(2 * loc + 1, std::move(left), std::make_pair(in.first, left.second /*last point excluded*/ - 1)) // otherwise split "in" between both subtrees
                                        , query(2 * loc + 2, std::move(right), std::make_pair(right.first, in.second)));
        }

        auto update(size_t loc, std::pair<size_t, size_t>&& cur, const std::pair<size_t, size_t>& at, const T& val) {
            if (cur.second <= at.first || cur.first > at.second) return; // exit if we have fallen out of bounds
            if (cur.second - cur.first == 1) { // update if we have reached a final leaf
                this->vec_[loc] = val;
                return;
            }
            size_t m = (cur.second - cur.first) / 2; // otherwise continue to split the interval into two pieces
            update(2 * loc + 1, std::make_pair(cur.first, cur.first + m), at, val);
            update(2 * loc + 2, std::make_pair(cur.first + m, cur.second), at, val);

            this->vec_[loc] = functional_type()(this->vec_[2 * loc + 1], this->vec_[2 * loc + 2]); // build the tree up again from the updated children
        }

        std::vector<T> vec_;
    };

    template <typename T, typename F>
    auto query(const segment<T, F>& s, const std::pair<size_t, size_t>& bounds) {
        auto r = s.vec_.size() / 4;
        return s.query(0, std::make_pair(0, r), std::make_pair(bounds.first, std::min(bounds.second, r - 1)));
    }

    template <typename T, typename F>
    auto update(segment<T, F>& s, const std::pair<size_t, size_t>& bounds, T&& val) {
        s.update(0, std::make_pair(0, s.vec_.size() / 4), bounds, val);
    }

    template <typename T, typename F>
    auto update(segment<T, F>& s, const std::pair<size_t, size_t>& bounds, const T& val) {
        s.update(0, std::make_pair(0, s.vec_.size() / 4), bounds, val);
    }
}