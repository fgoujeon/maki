//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace AOS(completion_transition_ns)
{
    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct go_on{};
    }

    namespace states
    {
        EMPTY_STATE(s0)
        EMPTY_STATE(s1)
        EMPTY_STATE(s2)
        EMPTY_STATE(s3)
        EMPTY_STATE(s4)
    }

    constexpr auto do_something = maki::action_ce([](context& ctx, maki::null_t /*event*/)
    {
        ctx.out = "did something";
    });

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,  states::s0)
        (states::s0, states::s1, maki::event<events::go_on>)
        (states::s1, states::s2, maki::null, do_something)
        (states::s2, states::s3, maki::event<events::go_on>)
        (states::s3, states::s4, maki::null)
        (states::s4, states::s0, maki::null)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            AOS_ASYNC_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST(completion_transition)
    {
        auto machine = machine_t{};

        AOS_CALL machine.AOS(start)();
        REQUIRE(machine.is<states::s0>());

        AOS_CALL machine.AOS(process_event)(events::go_on{});
        REQUIRE(machine.is<states::s2>());
        REQUIRE(machine.context().out == "did something");

        machine.context().out.clear();
        AOS_CALL machine.AOS(process_event)(events::go_on{});
        REQUIRE(machine.is<states::s0>());
    }
}
