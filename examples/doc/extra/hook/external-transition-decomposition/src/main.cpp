//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include <iostream>

template<class... Args>
void log_info(const Args&... args)
{
    (std::cout << ... << args) << '\n';
}

struct context
{
    bool log = false;
};

struct left_wing_sensor_signal{};
struct right_wing_sensor_signal{};
struct left_reactor_rotation{};
struct right_reactor_rotation{};
struct error{};

constexpr auto deploying_left_wing = maki::state_builder{};
constexpr auto deploying_right_wing = maki::state_builder{};
constexpr auto starting_left_reactor = maki::state_builder{};
constexpr auto starting_right_reactor = maki::state_builder{};
constexpr auto flying = maki::state_builder{};
constexpr auto failing = maki::state_builder{};

constexpr auto initializing_left_region_tt = maki::transition_table{}
    (maki::ini,             deploying_left_wing)
    (deploying_left_wing,   starting_left_reactor, maki::event<left_wing_sensor_signal>)
    (starting_left_reactor, maki::fin,             maki::event<left_reactor_rotation>)
;

constexpr auto initializing_right_region_tt = maki::transition_table{}
    (maki::ini,              deploying_right_wing)
    (deploying_right_wing,   starting_right_reactor, maki::event<right_wing_sensor_signal>)
    (starting_right_reactor, maki::fin,              maki::event<right_reactor_rotation>)
;

constexpr auto initializing = maki::state_builder{}.
    transition_tables
    (
        initializing_left_region_tt,
        initializing_right_region_tt
    )
;

constexpr auto transition_table = maki::transition_table{}
    (maki::ini,    initializing)
    (initializing, flying)
    (initializing, failing, maki::event<error>)
;

constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
    .pre_external_transition_hook_crset
    (
        [](context& ctx, const auto& region, const auto& source_state, const auto& /*event*/, const auto& target_state)
        {
            if(ctx.log)
            {
                log_info
                (
                    "Executing transition in ",
                    region.path().to_string(),
                    ": ",
                    source_state.pretty_name(),
                    " -> ",
                    target_state.pretty_name(),
                    "..."
                );
            }
        }
    )
    .post_external_transition_hook_crset
    (
        [](context& ctx, const auto& region, const auto& source_state, const auto& /*event*/, const auto& target_state)
        {
            if(ctx.log)
            {
                log_info
                (
                    "Executed transition in ",
                    region.path().to_string(),
                    ": ",
                    source_state.pretty_name(),
                    " -> ",
                    target_state.pretty_name(),
                    "."
                );
            }
        }
    )
;

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto machine = machine_t{};
    machine.process_event(left_wing_sensor_signal{});
    machine.context().log = true;
    machine.process_event(error{});
}
