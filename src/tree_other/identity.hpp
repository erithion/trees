/* 
*/
#pragma once

#include <functional>

namespace aux {

    // Defines an identity element w.r.t. the operation Op
    // The identity value must be defined either within a functor Op
    // or defined within a separately specialized identity class (see a specialization for std::plus below)
    template <typename Op>
    struct identity {
        const typename Op::result_type value = typename Op::identity;
    };

    template <typename T>
    struct identity<std::plus<T>> {
        const T value = T{};
    };
    template <typename T>
    struct identity<std::minus<T>> {
        const T value = T{};
    };

    template <typename T>
    struct identity<std::multiplies<T>> {
        const T value = 1;
    };
    template <typename T>
    struct identity<std::divides<T>> {
        const T value = 1;
    };

    template <typename T>
    struct identity<std::logical_and<T>> {
        const bool value = true;
    };
    template <typename T>
    struct identity<std::logical_or<T>> {
        const bool value = false;
    };

    template <typename T>
    struct identity<std::bit_and<T>> {
        const T value = static_cast<T>(0b1111111111111111111111111111111111111111111111111111111111111111);
    };
    template <typename T>
    struct identity<std::bit_or<T>> {
        const T value = false;
    };

    // TODO: add tests for all of them
}