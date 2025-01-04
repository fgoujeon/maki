//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#include <maki.hpp>
#include <type_traits>
#include <iostream>

struct context{};

//! [events]
struct digit_button_press{};
struct zero_button_press: digit_button_press{};
struct one_button_press: digit_button_press{};
struct two_button_press: digit_button_press{};
struct three_button_press: digit_button_press{};
//etc.

struct alpha_button_press{};
struct a_button_press: alpha_button_press{};
struct b_button_press: alpha_button_press{};
struct c_button_press: alpha_button_press{};
//etc.
//! [events]

//! [state-conf]
constexpr struct
{
    template<class Event>
    constexpr bool operator()(maki::type_t<Event> /*type*/) const
    {
        return std::is_base_of_v<digit_button_press, Event>;
    }
} is_digit_button_press;

constexpr auto my_state = maki::state_conf{}
    .internal_action_v
    (
        maki::any_if(is_digit_button_press),
        []
        {
            std::cout << "Beep\n";
        }
    )
;
//! [state-conf]

constexpr auto transition_table = maki::transition_table{}
    (my_state, maki::null, maki::null)
;

constexpr auto machine_conf = maki::machine_conf{}
    .context_a<context>()
    .transition_tables(transition_table)
;

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto machine = machine_t{};

    machine.process_event(one_button_press{});
    machine.process_event(two_button_press{});
    machine.process_event(three_button_press{});
    machine.process_event(a_button_press{}); //No beep
    machine.process_event(b_button_press{}); //No beep
    machine.process_event(c_button_press{}); //No beep
}
//! [all]
