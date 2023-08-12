//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace nullary_member_functions_ns
{
    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct e1
        {
        };

        struct e2
        {
        };
    }

    namespace states
    {
        EMPTY_STATE(off);

        struct on
        {
            using conf = maki::state_conf
                ::on_entry_any
                ::on_exit_any
            ;

            void on_entry(const events::e1&)
            {
                ctx.out += "on_entry(e1);";
            }

            void on_entry()
            {
                ctx.out += "on_entry();";
            }

            void on_exit(const events::e1&)
            {
                ctx.out += "on_exit(e1);";
            }

            void on_exit()
            {
                ctx.out += "on_exit();";
            }

            context& ctx;
        };
    }

    constexpr auto action = [](auto& /*machine*/, context& ctx, const auto& event)
    {
        using event_type = std::decay_t<decltype(event)>;
        if constexpr(std::is_same_v<event_type, events::e1>)
        {
            ctx.out += "execute(e1);";
        }
        else
        {
            ctx.out += "execute();";
        }
    };

    constexpr auto guard = [](auto& /*machine*/, context& ctx, const auto& event)
    {
        using event_type = std::decay_t<decltype(event)>;
        if constexpr(std::is_same_v<event_type, events::e1>)
        {
            ctx.out += "check(e1);";
        }
        else
        {
            ctx.out += "check();";
        }
        return true;
    };

    using transition_table_t = maki::transition_table
        ::add<states::off, events::e1, states::on,  action, guard>
        ::add<states::off, events::e2, states::on,  action, guard>
        ::add<states::on,  events::e1, states::off, action, guard>
        ::add<states::on,  events::e2, states::off, action, guard>
    ;

    struct machine_def
    {
        using conf = maki::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("nullary_member_functions")
{
    using namespace nullary_member_functions_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    ctx.out.clear();
    machine.process_event(events::e1{});
    REQUIRE(ctx.out == "check(e1);execute(e1);on_entry(e1);");

    ctx.out.clear();
    machine.process_event(events::e1{});
    REQUIRE(ctx.out == "check(e1);on_exit(e1);execute(e1);");

    ctx.out.clear();
    machine.process_event(events::e2{});
    REQUIRE(ctx.out == "check();execute();on_entry();");

    ctx.out.clear();
    machine.process_event(events::e2{});
    REQUIRE(ctx.out == "check();on_exit();execute();");
}
