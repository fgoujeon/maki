//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <memory>
#include <iostream>

struct some_component
{
    void print_value(const int value)
    {
        std::cout << "Value is " << value << '\n';
    }
};

//! [context]
struct my_context
{
    int some_value = 0;
    std::unique_ptr<some_component> pcomponent;
};
//! [context]

//! [state]
constexpr auto my_state = maki::state_conf{}
    .entry_action_c([](my_context& ctx)
    {
        ++ctx.some_value;
        ctx.pcomponent->print_value(ctx.some_value);
    })
;
//! [state]

// Just a dummy transition table, because we need one.
constexpr auto transition_table = maki::transition_table{}
    //source state, event,      target state
    (my_state,      maki::null, maki::null)
;

//! [machine-conf]
constexpr auto machine_conf = maki::machine_conf{}
    .context_a(maki::type<my_context>)
    .transition_tables(transition_table)
;
using machine_t = maki::make_machine<machine_conf>;
//! [machine-conf]

int main()
{
//! [machine-instance]
    auto machine = machine_t{41, std::make_unique<some_component>()};
//! [machine-instance]
}
//! [all]
