//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>

struct context{};

struct some_event
{
    int size = 0;
};

constexpr auto some_state = maki::state_builder{};
constexpr auto some_other_state = maki::state_builder{};

constexpr auto some_action = maki::action_v([]{});

//! [guards]
constexpr auto is_small = maki::guard_e([](const some_event& evt)
{
    return evt.size < 10;
});

constexpr auto is_large = maki::guard_e([](const some_event& evt)
{
    return evt.size > 1000;
});
//! [guards]

[[maybe_unused]]
//! [guard-composition]
constexpr auto is_average = maki::guard_e([](const some_event& evt)
{
    return !is_small.callable(evt) && !is_large.callable(evt);
});
//! [guard-composition]

[[maybe_unused]]
//! [better-guard-composition]
constexpr auto is_average_2 = !is_small && !is_large;
//! [better-guard-composition]

//! [better-guard-composition-in-tt]
constexpr auto transition_table = maki::transition_table{}
    (maki::ini,  some_state)
    (some_state, some_other_state, maki::event<some_event>, some_action, !is_small && !is_large)
;
//! [better-guard-composition-in-tt]

constexpr auto machine_conf = maki::machine_conf{}
    .context_a<context>()
    .transition_tables(transition_table)
;

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto mach = machine_t{};
    mach.process_event(some_event{});
}
