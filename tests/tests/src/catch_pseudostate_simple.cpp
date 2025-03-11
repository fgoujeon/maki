//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace catch_pseudostate_simple_ns
{
    struct context
    {
        std::string error;
    };

    struct button_press{};

    constexpr auto off = maki::state_builder{};
    constexpr auto on = maki::state_builder{};

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

    constexpr auto throw_runtime_error = maki::action_v
    (
        []
        {
            throw std::runtime_error{"Error"};
        }
    );

    constexpr auto transition_table = maki::transition_table{}
        (maki::init,   off)
        (off,          on,    maki::event<button_press>, throw_runtime_error)
        (maki::catch_, error, maki::event<std::exception_ptr>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("catch_pseudostate_simple")
{
    using namespace catch_pseudostate_simple_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    REQUIRE(machine.is<off>());

    ctx.error.clear();
    machine.process_event(button_press{});
    REQUIRE(machine.is<error>());
    REQUIRE(ctx.error == "Error");
}
