#pragma once
// Various iterator utils

namespace tree_search {

    namespace aux {

        template <typename Tree, typename T2 = typename Tree::node_type::value_type>
        struct value_type {
            using type = typename Tree::node_type::value_type;
        };

        template <typename Node>
        struct value_type<Node, typename Node::value_type > {
            using type = typename Node::value_type;
        };

        template <typename Tree, typename T2 = typename Tree::node_type>
        struct node_type {
            using type = typename Tree::node_type;
        };

        template <typename Node>
        struct node_type<Node, Node> {
            using type = typename Node;
        };

        template <typename Tree, typename T2 = typename Tree::node_type>
        struct ptr_type {
            using type = typename Tree::ptr_type;
        };

        template <typename Node>
        struct ptr_type<Node, Node> {
            using type = typename Node::ptr_type;
        };

        // Finds value_type of type T irrelevant to whether T is a tree node or the tree itself
        template <typename T>
        using value_type_t = typename value_type<T>::type;

        // Finds node_type of type T irrelevant to whether T is a tree node or the tree itself
        template <typename T>
        using node_type_t = typename node_type<T>::type;

        template <typename T>
        using ptr_type_t = typename ptr_type<T>::type;
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