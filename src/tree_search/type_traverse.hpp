#pragma once

namespace tree_search {

    namespace aux {

        template <typename T, typename ... Ts>
        constexpr bool is_one_of_v = std::disjunction_v<std::is_same<T, Ts>...>; // used in traverse/search
    }

    struct tag_inorder {};
    struct tag_preorder {};
    struct tag_postorder {};
}