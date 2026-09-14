//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace AOS(on_exception_ns)
{
    struct context
    {
        int always_zero = 0;
        std::string out;
    };

    namespace states
    {
        constexpr auto off = maki::state_mold{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "off::on_entry;";
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "off::on_exit;";
                }
            )
        ;

        constexpr auto on = maki::state_mold{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "on::on_entry;";

                    if(ctx.always_zero == 0) //We need this to avoid "unreachable code" warnings
                    {
                        throw std::runtime_error{"test;"};
                    }
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.out += "on::on_exit;";
                }
            )
        ;
    }

    namespace events
    {
        struct button_press{};

        struct exception
        {
            std::exception_ptr eptr;
        };
    }

    constexpr auto log_exception_default = maki::action_ce
    (
        [](context& ctx, const events::exception& event)
        {
            try
            {
                std::rethrow_exception(event.eptr);
            }
            catch(const std::exception& e)
            {
                ctx.out += e.what();
            }
        }
    );


    /*
    Default case (no catch)
    */

    constexpr auto transition_table_default = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on,  maki::event<events::button_press>)
        (states::on,  states::off, maki::event<events::button_press>)
    ;

    struct machine_conf_default
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table_default)
            .context_a<context>()
            AOS_MACHINE_OPTS
        ;
    };

    using sm_default_t = maki::machine<machine_conf_default>;


    /*
    Case with catch and transition from `undefined`
    */

    constexpr auto transition_table_with_trans = maki::transition_table{}
        (maki::ini,       states::off)
        (states::off,     states::on,  maki::event<events::button_press>)
        (states::on,      states::off, maki::event<events::button_press>)
        (maki::undefined, maki::null,  maki::event<events::exception>, log_exception_default)
    ;

    struct machine_conf_with_trans
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table_with_trans)
            .context_a<context>()
            .catch_mx
            (
                [](auto& mach, const std::exception_ptr& eptr) -> AOS_VOID
                {
                    AOS_CALL mach.AOS(process_event)(events::exception{eptr});
                }
            )
            AOS_MACHINE_OPTS
        ;
    };

    using sm_with_trans_t = maki::machine<machine_conf_with_trans>;


    /*
    Case with catch and transition from `all_states`
    */

    constexpr auto transition_table_with_all = maki::transition_table{}
        (maki::ini,        states::off)
        (states::off,      states::on,  maki::event<events::button_press>)
        (states::on,       states::off, maki::event<events::button_press>)
        (maki::all_states, maki::null,  maki::event<events::exception>, log_exception_default)
    ;

    struct machine_conf_with_all
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table_with_all)
            .context_a<context>()
            .catch_mx
            (
                [](auto& mach, const std::exception_ptr& eptr) -> AOS_VOID
                {
                    try
                    {
                        AOS_CALL mach.AOS(process_event_no_catch)(events::exception{eptr});
                    }
                    catch(...)
                    {
                    }
                }
            )
            AOS_MACHINE_OPTS
        ;
    };

    using sm_with_all_t = maki::machine<machine_conf_with_all>;


    template<class Mach>
    AOS_VOID test_with_catch()
    {
        auto machine = Mach{};
        auto& ctx = machine.context();

        AOS_CALL machine.AOS(start)();
        ctx.out.clear();

        AOS_CALL machine.AOS(process_event)(events::button_press{});
        CHECK(machine.template is<maki::undefined>());
        CHECK(ctx.out == "off::on_exit;on::on_entry;test;");
    }
}

namespace AOS(exception_default)
{
    AOS_TEST_CASE("exception_default")
    {
        using namespace AOS(on_exception_ns);

        {
            auto machine = sm_default_t{};
            auto& ctx = machine.context();

            AOS_CALL machine.AOS(start)();
            ctx.out.clear();

            CHECK_THROWS(AOS_CALL machine.AOS(process_event)(events::button_press{}));
            CHECK(machine.is<maki::undefined>());
            CHECK(ctx.out == "off::on_exit;on::on_entry;");
        }
    }
}

namespace AOS(exception_with_trans)
{
    AOS_TEST_CASE("exception_with_trans")
    {
        using namespace AOS(on_exception_ns);
        AOS_CALL test_with_catch<sm_with_trans_t>();
    }
}

namespace AOS(exception_with_all)
{
    AOS_TEST_CASE("exception_with_all")
    {
        using namespace AOS(on_exception_ns);
        AOS_CALL test_with_catch<sm_with_all_t>();
    }
}
