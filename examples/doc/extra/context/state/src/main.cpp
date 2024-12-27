//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <memory>
#include <iostream>

//! [context]
struct my_machine_context
{
    int value = 0;
};

class my_state_context
{
public:
    my_state_context(my_machine_context& parent):
        parent_(parent)
    {
    }

    void on_entry()
    {
        ++value_;
        parent_.value = value_;
    }

private:
    my_machine_context& parent_;
    int value_ = 0;
};

constexpr auto my_state = maki::state_conf{}
    .context_c(maki::type<my_state_context>)
    .entry_action_c(&my_state_context::on_entry)
;
//! [context]

// Just a dummy transition table, because we need one.
constexpr auto transition_table = maki::transition_table{}
    //source state, event,      target state
    (my_state,      maki::null, maki::null)
;

//! [machine-conf]
constexpr auto machine_conf = maki::machine_conf{}
    .context_a(maki::type<my_machine_context>)
    .transition_tables(transition_table)
;
using machine_t = maki::make_machine<machine_conf>;
//! [machine-conf]

int main()
{
//! [machine-instance]
    auto machine = machine_t{};
//! [machine-instance]
}
//! [all]
