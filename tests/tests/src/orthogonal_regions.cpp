//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace orthogonal_regions_ns
{
    struct context
    {
        int always_zero = 0;
        std::string out;
    };

    namespace events
    {
        struct button_press
        {
        };

        struct exception_request
        {
        };
    }

    namespace states
    {
        EMPTY_STATE(off0)
        EMPTY_STATE(off1)
        EMPTY_STATE(on0)
        constexpr auto on1 = maki::state_conf{}
            .internal_action_c<events::exception_request>
            (
                [](context& ctx)
                {
                    if(ctx.always_zero == 0) //We need this to avoid "unreachable code" warnings
                    {
                        throw std::runtime_error{"exception"};
                    }
                }
            )
        ;
    }

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables
        (
            maki::transition_table{}
                (states::off0, maki::event<events::button_press>, states::on0),
            maki::transition_table{}
                (states::off1, maki::event<events::button_press>, states::on1)
        )
        .context_a<context>()
        .exception_hook_mx
        (
            [](auto& mach, const std::exception_ptr& eptr)
            {
                try
                {
                    std::rethrow_exception(eptr);
                }
                catch(const std::exception& e)
                {
                    mach.context().out += std::string{"on_exception:"} + e.what() + ";";
                }
            }
        )
        .pre_state_transition_hook_crset
        (
            [](context& ctx, const auto& region_path, const auto /*source_state_constant*/, const auto& /*event*/, const auto /*target_state_constant*/)
            {
                ctx.out += "before_state_transition[" + region_path.to_string() + "];";
            }
        )
        .post_state_transition_hook_crset
        (
            [](context& ctx, const auto& region_path, const auto /*source_state_constant*/, const auto& /*event*/, const auto /*target_state_constant*/)
            {
                ctx.out += "after_state_transition[" + region_path.to_string() + "];";
            }
        )
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("orthogonal_regions")
{
    using namespace orthogonal_regions_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();
    const auto& region0 = machine.region<0>();
    const auto& region1 = machine.region<1>();

    machine.start();
    REQUIRE(region0.is<states::off0>());
    REQUIRE(region1.is<states::off1>());
    REQUIRE(ctx.out == "before_state_transition[machine_conf/0];after_state_transition[machine_conf/0];before_state_transition[machine_conf/1];after_state_transition[machine_conf/1];");

    ctx.out.clear();
    machine.process_event(events::button_press{});
    REQUIRE(region0.is<states::on0>());
    REQUIRE(region1.is<states::on1>());
    REQUIRE(ctx.out == "before_state_transition[machine_conf/0];after_state_transition[machine_conf/0];before_state_transition[machine_conf/1];after_state_transition[machine_conf/1];");

    ctx.out.clear();
    machine.process_event(events::exception_request{});
    REQUIRE(ctx.out == "on_exception:exception;");
}
