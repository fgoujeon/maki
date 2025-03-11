//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace catch_pseudostate_internal_transition_ns
{
    struct context
    {
        std::string out;
        std::string error;
    };

    struct button_press{};
    struct other_button_press{};

    constexpr auto off = maki::state_builder{};
    constexpr auto on = maki::state_builder{}
        .internal_action_v<other_button_press>
        (
            []
            {
                throw std::runtime_error{"Error"};
            }
        ).
        exit_action_c
        (
            [](context& ctx)
            {
                ctx.out += "exit;";
            }
        );

    constexpr auto error = maki::state_builder{}
        .entry_action_ce
        (
            [](context& ctx, const std::exception_ptr event)
            {
                try
                {
                    std::rethrow_exception(event);
                }
                catch(const std::exception& ex)
                {
                    ctx.error += ex.what();
                }
            }
        );

    constexpr auto transition_table = maki::transition_table{}
        (maki::init,   off)
        (off,          on,    maki::event<button_press>)
        (maki::catch_, error, maki::event<std::exception_ptr>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("catch_pseudostate_internal_transition")
{
    using namespace catch_pseudostate_internal_transition_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    CHECK(machine.is<off>());

    ctx.out.clear();
    ctx.error.clear();
    machine.process_event(button_press{});
    CHECK(machine.is<on>());
    CHECK(ctx.out == "");
    CHECK(ctx.error == "");

    ctx.out.clear();
    ctx.error.clear();
    machine.process_event(other_button_press{});
    CHECK(machine.is<error>());
    CHECK(ctx.out == "exit;");
    CHECK(ctx.error == "Error");
}
