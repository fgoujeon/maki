//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace catch_pseudostate_orthogonal_regions_ns
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

    constexpr auto transition_table_0 = maki::transition_table{}
        (maki::init,   off)
        (off,          on,    maki::event<button_press>, throw_runtime_error)
        (maki::catch_, error, maki::event<std::exception_ptr>)
    ;

    constexpr auto transition_table_1 = maki::transition_table{}
        (maki::init,   off)
        (off,          on,    maki::event<button_press>, throw_runtime_error)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table_0, transition_table_1)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("catch_pseudostate_orthogonal_regions")
{
    using namespace catch_pseudostate_orthogonal_regions_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    REQUIRE(machine.region<0>().is<off>());
    REQUIRE(machine.region<1>().is<off>());

    ctx.error.clear();
    machine.process_event(button_press{});
    REQUIRE(!machine.running());
    REQUIRE(ctx.error == "Error");
}
