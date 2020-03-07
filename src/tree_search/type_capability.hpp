#pragma once

namespace tree_search {

    struct capability_insert {};
    struct capability_rotate {};
    struct capability_search {};
    struct capability_traverse {};
    struct capability_remove {};

    struct capability_balance_manual {};

    struct capability_insert_interval {};
    struct capability_search_interval {};
    struct capability_remove_interval {};

    struct capability_insert_redblack {};
    struct capability_remove_redblack {};

    struct capability_insert_avl {};
    struct capability_remove_avl {};
}