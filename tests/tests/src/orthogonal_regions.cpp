//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
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
        EMPTY_STATE(off0);
        EMPTY_STATE(off1);
        EMPTY_STATE(on0);
        struct on1
        {
            using conf = maki::state_conf
                ::on_event<events::exception_request>
            ;

            void on_event(const events::exception_request&)
            {
                if(ctx.always_zero == 0) //We need this to avoid "unreachable code" warnings
                {
                    throw std::runtime_error{"exception"};
                }
            }

            context& ctx;
        };
    }

    struct machine_def
    {
        using conf = maki::machine_conf
            ::transition_tables
            <
                maki::transition_table
                    ::add<states::off0, events::button_press, states::on0>,
                maki::transition_table
                    ::add<states::off1, events::button_press, states::on1>
            >
            ::context<context>
            ::on_exception
            ::before_state_transition
            ::after_state_transition
        ;

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void before_state_transition(const Event& /*event*/)
        {
            constexpr auto region_index = maki::detail::tlu::get_t<RegionPath, 0>::region_index;
            ctx.out += "before_state_transition[" + std::to_string(region_index) + "];";
        }

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void after_state_transition(const Event& /*event*/)
        {
            constexpr auto region_index = maki::detail::tlu::get_t<RegionPath, 0>::region_index;
            ctx.out += "after_state_transition[" + std::to_string(region_index) + "];";
        }

        void on_exception(const std::exception_ptr& eptr)
        {
            try
            {
                std::rethrow_exception(eptr);
            }
            catch(const std::exception& e)
            {
                ctx.out += std::string{"on_exception:"} + e.what() + ";";
            }
        }

        context& ctx;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("orthogonal_regions")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    using machine_region_0_path = maki::region_path<machine_def, 0>;
    using machine_region_1_path = maki::region_path<machine_def, 1>;

    machine.start();
    REQUIRE(machine.is_active_state<machine_region_0_path, states::off0>());
    REQUIRE(machine.is_active_state<machine_region_1_path, states::off1>());
    REQUIRE(ctx.out == "before_state_transition[0];after_state_transition[0];before_state_transition[1];after_state_transition[1];");

    ctx.out.clear();
    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<machine_region_0_path, states::on0>());
    REQUIRE(machine.is_active_state<machine_region_1_path, states::on1>());
    REQUIRE(ctx.out == "before_state_transition[0];after_state_transition[0];before_state_transition[1];after_state_transition[1];");

    ctx.out.clear();
    machine.process_event(events::exception_request{});
    REQUIRE(ctx.out == "on_exception:exception;");
}
