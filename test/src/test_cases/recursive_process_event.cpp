//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm/fsm_fwd.hpp>

namespace
{
    struct fsm_configuration;
    using fsm = fgfsm::fsm<fsm_configuration>;
}

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context
    {
        std::string output;
    };

    namespace events
    {
        struct start_button_press{};

        struct quick_start_button_press{};

        struct end_of_loading{};

        struct self_call_request
        {
            std::string data;
        };

        struct self_call_response
        {
            std::string data;
        };
    }

    namespace states
    {
        struct idle
        {
            void on_entry()
            {
                ctx.output += "idle::on_entry;";
            }

            void on_exit()
            {
                ctx.output += "idle::on_exit;";
            }

            context& ctx;
        };

        struct loading
        {
            void on_entry()
            {
                ctx.output += "loading::on_entry;";
            }

            void on_exit()
            {
                ctx.output += "loading::on_exit;";
            }

            context& ctx;
        };

        struct ready
        {
            void on_entry()
            {
                ctx.output += "ready::on_entry;";
            }

            void on_event(const events::self_call_request& event)
            {
                sm.process_event(events::self_call_response{event.data});
            }

            void on_event(const events::self_call_response& event)
            {
                ctx.output = event.data;
            }

            void on_exit()
            {
                ctx.output += "ready::on_exit;";
            }

            context& ctx;
            fgfsm::fsm_ref<events::self_call_response> sm;
        };
    }

    namespace actions
    {
        struct skip_loading
        {
            void execute()
            {
                sm.process_event(events::end_of_loading{});
            }

            context& ctx;
            fgfsm::fsm_ref<events::end_of_loading> sm;
        };
    }

    struct fsm_configuration: fgfsm::fsm_configuration
    {
        using transition_table = fgfsm::transition_table
        <
            fgfsm::row<states::idle,    events::start_button_press,       states::loading>,
            fgfsm::row<states::idle,    events::quick_start_button_press, states::loading,  actions::skip_loading>,
            fgfsm::row<states::loading, events::end_of_loading,           states::ready>
        >;

        struct pre_transition_event_handler
        {
            void on_event(const events::quick_start_button_press&)
            {
                ctx.output += "quick_start_button_press;";
            }

            void on_event(const events::end_of_loading&)
            {
                ctx.output += "end_of_loading;";
            }

            context& ctx;
            fsm& sm;
        };
    };
}

TEST_CASE("recursive process_event")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    sm.process_event(events::quick_start_button_press{});
    REQUIRE
    (
        ctx.output ==
            "quick_start_button_press;"
            "idle::on_exit;"
            "loading::on_entry;"
            "end_of_loading;"
            "loading::on_exit;"
            "ready::on_entry;"
    );

    const auto long_string = "auienratuiernaturnietnrautisretadlepetwufieqarnpecdaedpceoanuprecanrpecadtpeadcepde";
    sm.process_event(events::self_call_request{long_string});
    REQUIRE(ctx.output == long_string);
}
