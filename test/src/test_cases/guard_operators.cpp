//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context
    {
        bool can_access_state0_0 = false;
        bool can_access_state0_1 = false;

        bool can_access_state1_0 = false;
        bool can_access_state1_1 = false;

        bool can_access_state2_0 = false;
        bool can_access_state2_1 = false;

        bool cant_access_state3 = true;
    };

    namespace states
    {
        FGFSM_SIMPLE_STATE(idle)
        FGFSM_SIMPLE_STATE(state0)
        FGFSM_SIMPLE_STATE(state1)
        FGFSM_SIMPLE_STATE(state2)
        FGFSM_SIMPLE_STATE(state3)
    }

    namespace events
    {
        struct start{};
        struct stop{};
    }

    namespace guards
    {

#define GUARD(NAME) \
    struct NAME \
    { \
        bool check() const \
        { \
            return ctx.NAME; \
        } \
 \
        context& ctx; \
    };

        GUARD(can_access_state0_0);
        GUARD(can_access_state0_1);
        GUARD(can_access_state1_0);
        GUARD(can_access_state1_1);
        GUARD(can_access_state2_0);
        GUARD(can_access_state2_1);
        GUARD(cant_access_state3);

#undef GUARD

        using can_access_state0 = fgfsm::and_<guards::can_access_state0_0, guards::can_access_state0_1>;
        using can_access_state1 = fgfsm::or_<guards::can_access_state1_0, guards::can_access_state1_1>;
        using can_access_state2 = fgfsm::xor_<guards::can_access_state2_0, guards::can_access_state2_1>;
        using can_access_state3 = fgfsm::not_<guards::cant_access_state3>;
    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::idle, events::start, states::state0, fgfsm::none, guards::can_access_state0>,
        fgfsm::row<states::idle, events::start, states::state1, fgfsm::none, guards::can_access_state1>,
        fgfsm::row<states::idle, events::start, states::state2, fgfsm::none, guards::can_access_state2>,
        fgfsm::row<states::idle, events::start, states::state3, fgfsm::none, guards::can_access_state3>,

        fgfsm::row<states::state0, events::stop, states::idle>,
        fgfsm::row<states::state1, events::stop, states::idle>,
        fgfsm::row<states::state2, events::stop, states::idle>,
        fgfsm::row<states::state3, events::stop, states::idle>
    >;

    using fsm = fgfsm::fsm<transition_table>;
}

TEST_CASE("guard operators")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    SECTION("and")
    {
       sm.process_event(events::stop{});
       ctx.can_access_state0_0 = false;
       ctx.can_access_state0_1 = false;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::idle>());

       sm.process_event(events::stop{});
       ctx.can_access_state0_0 = false;
       ctx.can_access_state0_1 = true;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::idle>());

       sm.process_event(events::stop{});
       ctx.can_access_state0_0 = true;
       ctx.can_access_state0_1 = false;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::idle>());

       sm.process_event(events::stop{});
       ctx.can_access_state0_0 = true;
       ctx.can_access_state0_1 = true;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::state0>());
    }

    SECTION("or")
    {
       sm.process_event(events::stop{});
       ctx.can_access_state1_0 = false;
       ctx.can_access_state1_1 = false;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::idle>());

       sm.process_event(events::stop{});
       ctx.can_access_state1_0 = false;
       ctx.can_access_state1_1 = true;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::state1>());

       sm.process_event(events::stop{});
       ctx.can_access_state1_0 = true;
       ctx.can_access_state1_1 = false;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::state1>());

       sm.process_event(events::stop{});
       ctx.can_access_state1_0 = true;
       ctx.can_access_state1_1 = true;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::state1>());
    }

    SECTION("xor")
    {
       sm.process_event(events::stop{});
       ctx.can_access_state2_0 = false;
       ctx.can_access_state2_1 = false;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::idle>());

       sm.process_event(events::stop{});
       ctx.can_access_state2_0 = false;
       ctx.can_access_state2_1 = true;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::state2>());

       sm.process_event(events::stop{});
       ctx.can_access_state2_0 = true;
       ctx.can_access_state2_1 = false;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::state2>());

       sm.process_event(events::stop{});
       ctx.can_access_state2_0 = true;
       ctx.can_access_state2_1 = true;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::idle>());
    }

    SECTION("not")
    {
       sm.process_event(events::stop{});
       ctx.cant_access_state3 = true;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::idle>());

       sm.process_event(events::stop{});
       ctx.cant_access_state3 = false;
       sm.process_event(events::start{});
       REQUIRE(sm.is_active_state<states::state3>());
    }
}
