//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common/catch.hpp"

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
        struct idle{};
        struct state0{};
        struct state1{};
        struct state2{};
        struct state3{};
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

        using can_access_state0 = awesm::and_<guards::can_access_state0_0, guards::can_access_state0_1>;
        using can_access_state1 = awesm::or_<guards::can_access_state1_0, guards::can_access_state1_1>;
        using can_access_state2 = awesm::xor_<guards::can_access_state2_0, guards::can_access_state2_1>;
        using can_access_state3 = awesm::not_<guards::cant_access_state3>;
    }

    using sm_transition_table = awesm::transition_table
    <
        awesm::row<states::idle, events::start, states::state0, awesm::none, guards::can_access_state0>,
        awesm::row<states::idle, events::start, states::state1, awesm::none, guards::can_access_state1>,
        awesm::row<states::idle, events::start, states::state2, awesm::none, guards::can_access_state2>,
        awesm::row<states::idle, events::start, states::state3, awesm::none, guards::can_access_state3>,

        awesm::row<states::state0, events::stop, states::idle>,
        awesm::row<states::state1, events::stop, states::idle>,
        awesm::row<states::state2, events::stop, states::idle>,
        awesm::row<states::state3, events::stop, states::idle>
    >;

    struct sm_def
    {
        using conf = awesm::sm_conf<awesm::transition_table_list<sm_transition_table>>;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("guard operators")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();

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
