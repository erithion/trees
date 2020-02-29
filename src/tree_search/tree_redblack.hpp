#pragma once

#include "type_capability.hpp"
#include "capability_rotate.hpp"

#include <memory>

namespace tree_search {

    namespace aux {

        enum rb_color {
            rb_black = 0,
            rb_red = 1
        };

        template <typename, typename>
        struct redblack_augment {
            rb_color color_ = rb_red;
        };
    }
}