//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include <iostream>

struct context{};

struct E{};

constexpr auto A = maki::action_v([]{});

//! [internal-transition-in-state-builder]
constexpr auto ST = maki::state_builder{}
    .internal_action_v<E>([]
    {
        //...
    })
;
//! [internal-transition-in-state-builder]

constexpr auto S = maki::state_builder{};
constexpr auto T = maki::state_builder{};

constexpr auto transition_table = maki::transition_table{}
    (maki::init, S)

//! [external-transition]
(S, T, maki::event<E>)
//! [external-transition]

//! [completion-transition]
(S, T, maki::null)
//! [completion-transition]

//! [external-self-transition]
(ST, ST, maki::event<E>)
//! [external-self-transition]

//! [external-completion-self-transition]
(ST, ST, maki::null)
//! [external-completion-self-transition]

//! [internal-transition]
(ST, maki::null, maki::event<E>, A)
//! [internal-transition]

//! [local-transition-to-substate]
(maki::all_states, T, maki::event<E>)
//! [local-transition-to-substate]
;

constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
;

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto machine = machine_t{};
    return 0;
}
