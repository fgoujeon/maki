//Copyright Florian Goujeon 2021 - 2023.
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
                .add_c<states::off0, events::button_press, states::on0>,
            maki::transition_table{}
                .add_c<states::off1, events::button_press, states::on1>
        )
        .context<context>()
        .exception_action_me
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
        .pre_state_transition_action_crset
        (
            [](context& ctx, const auto& path_constant, const auto /*source_state_constant*/, const auto& /*event*/, const auto /*target_state_constant*/)
            {
                const auto region_index = path_constant.value.template at<1>();
                ctx.out += "before_state_transition[" + std::to_string(region_index) + "];";
            }
        )
        .post_state_transition_action_crset
        (
            [](context& ctx, const auto& path_constant, const auto /*source_state_constant*/, const auto& /*event*/, const auto /*target_state_constant*/)
            {
                const auto region_index = path_constant.value.template at<1>();
                ctx.out += "after_state_transition[" + std::to_string(region_index) + "];";
            }
        )
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("orthogonal_regions")
{
    using namespace orthogonal_regions_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    static constexpr auto machine_region_0_path = maki::path{0};
    static constexpr auto machine_region_1_path = maki::path{1};

    machine.start();
    REQUIRE(machine.active_state<machine_region_0_path, states::off0>());
    REQUIRE(machine.active_state<machine_region_1_path, states::off1>());
    REQUIRE(ctx.out == "before_state_transition[0];after_state_transition[0];before_state_transition[1];after_state_transition[1];");

    ctx.out.clear();
    machine.process_event(events::button_press{});
    REQUIRE(machine.active_state<machine_region_0_path, states::on0>());
    REQUIRE(machine.active_state<machine_region_1_path, states::on1>());
    REQUIRE(ctx.out == "before_state_transition[0];after_state_transition[0];before_state_transition[1];after_state_transition[1];");

    ctx.out.clear();
    machine.process_event(events::exception_request{});
    REQUIRE(ctx.out == "on_exception:exception;");
}
