//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace
{
    struct machine_def;
    using machine_t = maki::machine<machine_def>;

    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct button_press
        {
            std::string data;
        };

        struct internal
        {
            std::string data;
        };
    }

    namespace states
    {
        EMPTY_STATE(off);

        struct on_0
        {
            static constexpr auto conf = maki::state_conf_c
                .enable_on_entry()
                .enable_on_event_for<events::internal>()
                .enable_on_exit()
            ;

            void on_entry(const events::button_press& event)
            {
                ctx.out += event.data + "2";
            }

            void on_event(const events::internal& event)
            {
                ctx.out += event.data + "2";
            }

            void on_exit(const events::button_press& event)
            {
                ctx.out += event.data + "1";
            }

            context& ctx;
        };

        constexpr auto on_transition_table = maki::transition_table_c
            .add<states::on_0, events::button_press, maki::null>
        ;

        struct on
        {
            static constexpr auto conf = maki::submachine_conf_c
                .set_transition_tables(on_transition_table)
                .enable_on_entry()
                .enable_on_event_for<events::internal>()
                .enable_on_exit()
            ;

            void on_entry(const events::button_press& event)
            {
                ctx.out += event.data + "1";
            }

            void on_event(const events::internal& event)
            {
                ctx.out += event.data + "1";
            }

            void on_exit(const events::button_press& event)
            {
                ctx.out += event.data + "2";
            }

            context& ctx;
        };
    }

    constexpr auto transition_table = maki::transition_table_c
        .add<states::off, events::button_press, states::on>
        .add<states::on,  events::button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables(transition_table)
            .set_context_type<context>()
        ;
    };
}

TEST_CASE("submachine_on_xxx")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    machine.process_event(events::button_press{"a"});
    REQUIRE(ctx.out == "a1a2");

    ctx.out.clear();
    machine.process_event(events::internal{"b"});
    REQUIRE(ctx.out == "b1b2");

    ctx.out.clear();
    machine.process_event(events::button_press{"c"});
    REQUIRE(ctx.out == "c1c2");
}
