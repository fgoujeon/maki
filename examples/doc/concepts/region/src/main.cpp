//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>

struct context{};

struct left_wing_deployment{};

struct right_wing_deployment{};

constexpr auto deploying_left_wing = maki::state_builder{};
constexpr auto deploying_right_wing = maki::state_builder{};
constexpr auto starting_left_reactor = maki::state_builder{};
constexpr auto starting_right_reactor = maki::state_builder{};

//! [regions]
constexpr auto initializing_left_region_tt = maki::transition_table{}
    (maki::init,          deploying_left_wing)
    (deploying_left_wing, starting_left_reactor, maki::event<left_wing_deployment>)
;

constexpr auto initializing_right_region_tt = maki::transition_table{}
    (maki::init,           deploying_right_wing)
    (deploying_right_wing, starting_right_reactor, maki::event<right_wing_deployment>)
;

constexpr auto initializing = maki::state_builder{}
    .transition_tables(initializing_left_region_tt, initializing_right_region_tt)
;
//! [regions]

constexpr auto transition_table = maki::transition_table{}
    (maki::init, initializing)
;

constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
;

using machine_t = maki::machine<machine_conf>;

int main()
{
}
