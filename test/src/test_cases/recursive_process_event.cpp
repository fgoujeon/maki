//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    class robot;
    void process_event(robot& r, const fgfsm::event_ref& evt);

    struct context
    {
        context(robot& rob):
            rob(rob)
        {
        }

        void process_event(const fgfsm::event_ref& evt)
        {
            ::process_event(rob, evt);
        }

        robot& rob;
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
            template<class StartState, class TargetState>
            void operator()(StartState&, const fgfsm::event_ref&, TargetState&)
            {
                ctx.process_event(events::end_of_loading{});
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

    class robot
    {
        private:
            using fsm = fgfsm::fsm<transition_table>;

        public:
            robot():
                ctx_{*this},
                fsm_{ctx_}
            {
            }

            void process_event(const fgfsm::event_ref& evt)
            {
                fsm_.process_event(evt);
            }

            const std::string get_output() const
            {
                return ctx_.output;
            }

        private:
            context ctx_;
            fsm fsm_;
    };

    void process_event(robot& r, const fgfsm::event_ref& evt)
    {
        r.process_event(evt);
    }
}

TEST_CASE("recursive process_event")
{
    auto rob = robot{};

    rob.process_event(events::quick_start_button_press{});
    REQUIRE(rob.get_output() == "idle::on_exit;loading::on_entry;loading::on_exit;ready::on_entry;");
}
