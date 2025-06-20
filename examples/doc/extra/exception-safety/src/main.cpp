//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>

struct context{};

struct some_event{};

struct error
{
    std::exception_ptr eptr;
};

constexpr auto off = maki::state_mold{};
constexpr auto on = maki::state_mold{};

constexpr auto transition_table = maki::transition_table{}
    (maki::ini, off)
    (off,       on)
;

//! [catch]
constexpr auto machine_conf = maki::machine_conf{}
    .context_a<context>()
    .transition_tables(transition_table)
    .catch_mx([](auto& mach, const std::exception_ptr& eptr)
    {
        /*
        Note: The possibility of the call below to throw in its turn should also
        be taken care of.
        */

        mach.process_event_no_catch(error{eptr});
    })
;
//! [catch]

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto mach = machine_t{};
    mach.process_event(some_event{});
}
