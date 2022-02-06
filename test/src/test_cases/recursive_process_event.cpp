//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    class fsm;
    void process_event(fsm& sm, const fgfsm::any_cref& event);

    struct context
    {
        context(fsm& sm):
            sm(sm)
        {
        }

        void process_event(const fgfsm::any_cref& event)
        {
            ::process_event(sm, event);
        }

        fsm& sm;
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
            void on_entry(const fgfsm::any_cref&)
            {
                ctx.output += "idle::on_entry;";
            }

            void on_event(const fgfsm::any_cref&){}

            void on_exit(const fgfsm::any_cref&)
            {
                ctx.output += "idle::on_exit;";
            }

            context& ctx;
        };

        struct loading
        {
            void on_entry(const fgfsm::any_cref&)
            {
                ctx.output += "loading::on_entry;";
            }

            void on_event(const fgfsm::any_cref&){}

            void on_exit(const fgfsm::any_cref&)
            {
                ctx.output += "loading::on_exit;";
            }

            context& ctx;
        };

        struct ready
        {
            void on_entry(const fgfsm::any_cref&)
            {
                ctx.output += "ready::on_entry;";
            }

            void on_event(const fgfsm::any_cref& event)
            {
                visit
                (
                    event,
                    [this](const events::self_call_request& event)
                    {
                        ctx.process_event(events::self_call_response{event.data});
                    },
                    [this](const events::self_call_response& event)
                    {
                        ctx.output = event.data;
                    }
                );
            }

            void on_exit(const fgfsm::any_cref&)
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
            void operator()(StartState&, const fgfsm::any_cref&, TargetState&)
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

    class fsm
    {
        public:
            fsm():
                ctx_{*this},
                impl_{ctx_}
            {
            }

            void process_event(const fgfsm::any_cref& event)
            {
                impl_.process_event(event);
            }

            const context& get_context() const
            {
                return ctx_;
            }

        private:
            context ctx_;
            fgfsm::fsm<transition_table> impl_;
    };

    void process_event(fsm& sm, const fgfsm::any_cref& event)
    {
        sm.process_event(event);
    }
}

TEST_CASE("recursive process_event")
{
    auto sm = fsm{};
    const auto& ctx = sm.get_context();

    sm.process_event(events::quick_start_button_press{});
    REQUIRE(ctx.output == "idle::on_exit;loading::on_entry;loading::on_exit;ready::on_entry;");

    const auto long_string = "auienratuiernaturnietnrautisretadlepetwufieqarnpecdaedpceoanuprecanrpecadtpeadcepde";
    sm.process_event(events::self_call_request{long_string});
    REQUIRE(ctx.output == long_string);
}
