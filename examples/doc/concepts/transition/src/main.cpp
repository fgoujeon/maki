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

//! [internal-transition-in-state-conf]
constexpr auto ST = maki::state_conf{}
    .internal_action_v<E>([]
    {
        //...
    })
;
//! [internal-transition-in-state-conf]

constexpr auto S = maki::state_conf{};
constexpr auto T = maki::state_conf{};

constexpr auto transition_table = maki::transition_table{}
//! [completion-transition]
    (S, maki::null, T)
//! [completion-transition]

//! [external-self-transition]
    (ST, maki::event<E>, ST)
//! [external-self-transition]

//! [internal-transition]
    (ST, maki::event<E>, maki::null, A)
//! [internal-transition]

//! [regular-local-transition-from-parent]
    (maki::all_states, maki::event<E>, T)
//! [regular-local-transition-from-parent]
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
