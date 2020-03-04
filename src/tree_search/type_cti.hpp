#pragma once
// Compile time info for trees

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

        template <typename T, typename ... Ts>
        constexpr bool is_one_of_v = std::disjunction_v<std::is_same<T, Ts>...>; // used in traverse/search
    }
}