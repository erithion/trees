#pragma once

#include "type_capability.hpp"
#include "tree_redblack.hpp"
#include "tree_interval.hpp"
#include "tree_avl.hpp"
#include "type_cti.hpp"

#include <type_traits>

#define Stringize( L )     #L 
#define MakeString( M, L ) M(L)
#define $Line MakeString( Stringize, __LINE__ )
#define Src __FILE__ "(" $Line ") : "

namespace tree_search {

    namespace aux {

        struct show_warning_redblack {

            template <typename T, std::enable_if_t<std::is_same_v<T, std::true_type>, int> = 0>
            show_warning_redblack(T) {}

            template <typename T, std::enable_if_t<std::is_same_v<T, std::false_type>, int> = 0>
            show_warning_redblack(T) {
#if defined(__GNUC__) || defined(__GNUG__)
                #warning "You are using 'autobalanced' redblack_augment with 'manually balanced' insert/remove capabilities. If it is intentional, use capability_supress_warning_redblack."
#elif defined(_MSC_VER)
#pragma message (Src "Library warning: You are using 'autobalanced' redblack_augment with 'manually balanced' insert/remove capabilities. If it is intentional, use capability_supress_warning_redblack.")
#endif
            }
        };
        struct show_warning_interval {

            template <typename T, std::enable_if_t<std::is_same_v<T, std::true_type>, int> = 0>
            show_warning_interval(T) {}

            template <typename T, std::enable_if_t<std::is_same_v<T, std::false_type>, int> = 0>
            show_warning_interval(T) {
#if defined(__GNUC__) || defined(__GNUG__)
                #warning "You are using 'autobalanced' interval_augment with 'manually balanced' insert/remove capabilities. If it is intentional, use capability_supress_warning_interval."
#elif defined(_MSC_VER)
#pragma message (Src "Library warning: You are using 'autobalanced' interval_augment with 'manually balanced' insert/remove capabilities. If it is intentional, use capability_supress_warning_interval.")
#endif
            }
        };
        struct show_warning_avl {

            template <typename T, std::enable_if_t<std::is_same_v<T, std::true_type>, int> = 0>
            show_warning_avl(T) {}

            template <typename T, std::enable_if_t<std::is_same_v<T, std::false_type>, int> = 0>
            show_warning_avl(T) {
#if defined(__GNUC__) || defined(__GNUG__)
                #warning "You are using 'autobalanced' avl_augment with 'manually balanced' insert/remove capabilities. If it is intentional, use capability_supress_warning_avl."
#elif defined(_MSC_VER)
#pragma message (Src "Library warning: You are using 'autobalanced' avl_augment with 'manually balanced' insert/remove capabilities. If it is intentional, use capability_supress_warning_avl.")
#endif
            }
        };

        template <typename Augment, typename ... Capabilities>
        struct warning {};

        template <typename Element, typename Node, typename ... Capabilities>
        struct warning<redblack_augment<Element, Node>, Capabilities...> {

            using condition_t = typename std::integral_constant<bool, 
                (!is_one_of_v<capability_insert, Capabilities...> 
                    && !is_one_of_v<capability_remove, Capabilities...>)
                 || is_one_of_v<capability_supress_warning_redblack, Capabilities...>
            >::type;
            show_warning_redblack a = { condition_t{} };
        };

        template <typename Element, typename Node, typename ... Capabilities>
        struct warning<interval_augment<Element, Node>, Capabilities...> {

            using condition_t = typename std::integral_constant<bool,
                (!is_one_of_v<capability_insert, Capabilities...>
                    && !is_one_of_v<capability_remove, Capabilities...>)
                || is_one_of_v<capability_supress_warning_interval, Capabilities...>
            >::type;
            show_warning_interval a = { condition_t{} };
        };

        template <typename Element, typename Node, typename ... Capabilities>
        struct warning<avl_augment<Element, Node>, Capabilities...> {

            using condition_t = typename std::integral_constant<bool,
                (!is_one_of_v<capability_insert, Capabilities...> 
                    && !is_one_of_v<capability_remove, Capabilities...>)
                || is_one_of_v<capability_supress_warning_avl, Capabilities...>
            >::type;
            show_warning_avl a = { condition_t{} };
        };
    }
}