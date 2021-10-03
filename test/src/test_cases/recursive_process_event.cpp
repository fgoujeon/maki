//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    FGFSM_DECLARE_FSM_CLASS(fsm)

    struct context
    {
        fsm* pfsm = nullptr;
        std::string output;
    };

    namespace events
    {
        struct start_button_press{};
        struct quick_start_button_press{};
        struct end_of_loading{};
    }

    namespace states
    {
        struct idle
        {
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

            void on_exit()
            {
                ctx.output += "ready::on_exit;";
            }

            context& ctx;
        };
    }

    namespace actions
    {
        struct skip_loading
        {
            template<class StartState, class Event, class TargetState>
            void operator()(StartState&, Event&, TargetState&)
            {
                process_event(ctx.pfsm, events::end_of_loading{});
            }

            context& ctx;
        };
    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::idle,    events::start_button_press,       states::loading>,
        fgfsm::row<states::idle,    events::quick_start_button_press, states::loading,  actions::skip_loading>,
        fgfsm::row<states::loading, events::end_of_loading,           states::ready>
    >;

    using fsm_impl = fgfsm::fsm<transition_table>;
    FGFSM_DEFINE_FSM_CLASS(fsm, fsm_impl)
}

TEST_CASE("recursive process_event")
{
    auto ctx = context{};
    auto sm = fsm{ctx};
    ctx.pfsm = &sm;

    sm.process_event(events::quick_start_button_press{});
    REQUIRE(ctx.output == "idle::on_exit;loading::on_entry;loading::on_exit;ready::on_entry;");
}
