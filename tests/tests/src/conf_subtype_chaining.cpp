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

#define STATE_OPTIONS \
    X(pretty_name) \
    X(on_event<>) \
    X(on_event_auto) \
    X(on_entry_any) \
    X(on_exit_any)

#define SUBMACHINE_OPTIONS \
    STATE_OPTIONS \
    X(context<context>) \
    X(transition_tables<>)

#define MACHINE_OPTIONS \
    SUBMACHINE_OPTIONS \
    X(after_state_transition) \
    X(no_auto_start) \
    X(before_state_transition) \
    X(no_run_to_completion) \
    X(on_exception) \
    X(on_unprocessed) \
    X(small_event_max_align<0>) \
    X(small_event_max_size<1>)

#define X(option) awesm::detail::options::option,
    using state_conf_tpl_1_t = awesm::state_conf_tpl
    <
        STATE_OPTIONS
        awesm::detail::options::pretty_name
    >;
#undef X

#define X(option) ::option
    using state_conf_tpl_2_t = awesm::state_conf
        STATE_OPTIONS
        ::pretty_name
    ;
#undef X

#define X(option) awesm::detail::options::option,
    using submachine_conf_tpl_1_t = awesm::submachine_conf_tpl
    <
        SUBMACHINE_OPTIONS
        awesm::detail::options::pretty_name
    >;
#undef X

#define X(option) ::option
    using submachine_conf_tpl_2_t = awesm::submachine_conf
        SUBMACHINE_OPTIONS
        ::pretty_name
    ;
#undef X

#define X(option) awesm::detail::options::option,
    using machine_conf_tpl_1_t = awesm::machine_conf_tpl
    <
        MACHINE_OPTIONS
        awesm::detail::options::pretty_name
    >;
#undef X

#define X(option) ::option
    using machine_conf_tpl_2_t = awesm::machine_conf
        MACHINE_OPTIONS
        ::pretty_name
    ;
#undef X

#undef MACHINE_OPTIONS
#undef SUBMACHINE_OPTIONS
#undef STATE_OPTIONS
}

TEST_CASE("conf_subtype_chaining")
{
    REQUIRE(std::is_same_v<state_conf_tpl_1_t, state_conf_tpl_2_t>);
    REQUIRE(std::is_same_v<submachine_conf_tpl_1_t, submachine_conf_tpl_2_t>);
    REQUIRE(std::is_same_v<machine_conf_tpl_1_t, machine_conf_tpl_2_t>);
}
