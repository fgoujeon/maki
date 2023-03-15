//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct context{};

#define OPTIONS \
    X(after_state_transition) \
    X(no_auto_start) \
    X(before_entry) \
    X(before_state_transition) \
    X(context<context>) \
    X(no_run_to_completion) \
    X(get_pretty_name) \
    X(on_exception) \
    X(on_unprocessed) \
    X(small_event_max_align<0>) \
    X(small_event_max_size<1>) \
    X(on_event<>) \
    X(on_event_auto) \
    X(on_entry<>) \
    X(on_exit<>)

    using sm_conf_1_t = awesm::sm_conf
    <
#define X(option) awesm::sm_opts::option,
    OPTIONS
#undef X
        awesm::sm_opts::transition_tables<>
    >;

    using sm_conf_2_t = awesm::sm_conf<>
#define X(option) ::option
    OPTIONS
#undef X
        ::transition_tables<>
    ;

#undef OPTIONS
}

TEST_CASE("conf_subtype_chaining")
{
    REQUIRE(std::is_same_v<sm_conf_1_t, sm_conf_2_t>);
}
