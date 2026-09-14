//Copyright Florian Goujeon 2021 - 2026.
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
        std::string output;
    };

    namespace events
    {
        struct s0_to_s1_request{};
        struct s1_to_s2_request{};
        struct s2_to_s0_request{};
    }

    namespace states
    {
        constexpr auto s0 = maki::state_mold{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s0::on_entry;";
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s0::on_exit;";
                }
            )
        ;

        constexpr auto s1 = maki::state_mold{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s1::on_entry;";
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s1::on_exit;";
                }
            )
        ;

        constexpr auto s2 = maki::state_mold{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s2::on_entry;";
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.output += "s2::on_exit;";
                }
            )
        ;
    }

    namespace actions
    {
#if AOS_ASYNC
        using machine_ref_s1_to_s2 = maki::any_async_machine_ref_e<boost::cobalt::promise, events::s1_to_s2_request>;
#else
        using machine_ref_s1_to_s2 = maki::machine_ref_e<events::s1_to_s2_request>;
#endif

#if AOS_ASYNC
        using machine_ref_s2_to_s0 = maki::any_async_machine_ref_e<boost::cobalt::promise, events::s2_to_s0_request>;
#else
        using machine_ref_s2_to_s0 = maki::machine_ref_e<events::s2_to_s0_request>;
#endif

        constexpr auto s0_to_s1 = maki::action_m([]
        (
            machine_ref_s1_to_s2 mach
        ) -> AOS_VOID
        {
            AOS_CALL mach.AOS(process_event)(events::s1_to_s2_request{});
        });

        constexpr auto s1_to_s2 = maki::action_m([]
        (
            machine_ref_s2_to_s0 mach
        ) -> AOS_VOID
        {
            AOS_CALL mach.AOS(process_event)(events::s2_to_s0_request{});
        });
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,  states::s0)
        (states::s0, states::s1, maki::event<events::s0_to_s1_request>, actions::s0_to_s1)
        (states::s1, states::s2, maki::event<events::s1_to_s2_request>, actions::s1_to_s2)
        (states::s2, states::s0, maki::event<events::s2_to_s0_request>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            AOS_MACHINE_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST_CASE("recursive_process_event")
    {
        auto machine = machine_t{};
        auto& ctx = machine.context();

        AOS_CALL machine.AOS(start)();
        REQUIRE(ctx.output == "s0::on_entry;");

        //Indirectly process s1_to_s2_request and s2_to_s0_request
        ctx.output.clear();
        AOS_CALL machine.AOS(process_event)(events::s0_to_s1_request{});
        REQUIRE
        (
            ctx.output ==
                "s0::on_exit;"
                "s1::on_entry;"
                "s1::on_exit;"
                "s2::on_entry;"
                "s2::on_exit;"
                "s0::on_entry;"
        );
    }
}
