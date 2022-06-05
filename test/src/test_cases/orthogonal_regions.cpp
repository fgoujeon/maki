//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context
    {
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
        struct off0{};
        struct off1{};
        struct on0{};
        struct on1
        {
            void on_event(const events::exception_request&)
            {
                throw std::runtime_error{"exception"};
            }
        };
    }

    struct region_conf_0: awesm::region_configuration
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::off0, events::button_press, states::on0>
        >;
    };

    struct region_conf_1: awesm::region_configuration
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::off1, events::button_press, states::on1>
        >;
    };

    struct sm_configuration: awesm::sm_configuration
    {
        using region_configurations = std::tuple
        <
            region_conf_0,
            region_conf_1
        >;

        template<class Sm>
        struct exception_handler
        {
            void on_exception(const std::exception_ptr& e)
            {
                try
                {
                    std::rethrow_exception(e);
                }
                catch(const std::exception& e)
                {
                    ctx.out += std::string{"on_exception:"} + e.what() + ";";
                }
            }

            Sm& sm;
            context& ctx;
        };

        template<class Sm>
        struct state_transition_hook_set
        {
            template<class SourceState, class Event, class TargetState>
            void before_transition(const Event& /*event*/)
            {
                ctx.out += "before_transition;";
            }

            template<class SourceState, class Event, class TargetState>
            void after_transition(const Event& /*event*/)
            {
                ctx.out += "after_transition;";
            }

            Sm& sm;
            context& ctx;
        };
    };

    using sm_t = awesm::sm<sm_configuration>;
}

TEST_CASE("orthogonal_regions")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    REQUIRE(sm.is_active_state<states::off0, 0>());
    REQUIRE(sm.is_active_state<states::off1, 1>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on0, 0>());
    REQUIRE(sm.is_active_state<states::on1, 1>());
    REQUIRE(ctx.out == "before_transition;after_transition;before_transition;after_transition;");

    ctx.out.clear();
    sm.process_event(events::exception_request{});
    REQUIRE(ctx.out == "on_exception:exception;");
}
