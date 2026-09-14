//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include "common.hpp"
#include <maki.hpp>

namespace AOS(small_event_requirements)
{
    enum class new_operator_type
    {
        plain,
        placement
    };

    auto called_new_operator_type = new_operator_type::plain;

    struct context
    {
    };

    template<class T>
    struct event
    {
        T data;

        static void* operator new(size_t size)
        {
            called_new_operator_type = new_operator_type::plain;
            return ::operator new(size);
        }

        template<class Storage>
        static void* operator new(size_t size, Storage& storage)
        {
            called_new_operator_type = new_operator_type::placement;
            return ::operator new(size, storage);
        }

        static void operator delete(void* ptr)
        {
            ::operator delete(ptr);
        }
    };

    template<class Event>
    struct event_processing_request
    {
    };

    using small_event = event<int>;
    using big_event = event<long double>;

    constexpr auto state = maki::state_mold{};

    template<class Event>
    inline constexpr auto process_event = maki::action_m([](auto& machine) -> AOS_VOID
    {
        AOS_CALL machine.AOS(process_event)(Event{});
    });

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,  state)
        (state, maki::null, maki::event<event_processing_request<small_event>>, process_event<small_event>)
        (state, maki::null, maki::event<event_processing_request<big_event>>, process_event<big_event>)
    ;

    template<size_t SmallEventMaxSize, size_t SmallEventMaxAlign>
    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            .small_event_max_size(SmallEventMaxSize)
            .small_event_max_align(SmallEventMaxAlign)
            AOS_MACHINE_OPTS
        ;
    };

    template<size_t SmallEventMaxSize, size_t SmallEventMaxAlign>
    using machine_t = maki::machine<machine_conf<SmallEventMaxSize, SmallEventMaxAlign>>;

    template<size_t SmallEventMaxSize, size_t SmallEventMaxAlign>
    AOS_VOID test
    (
        const new_operator_type expected_new_operator_type_for_small_event,
        const new_operator_type expected_new_operator_type_for_big_event
    )
    {
        auto machine = machine_t<SmallEventMaxSize, SmallEventMaxAlign>{};

        AOS_CALL machine.AOS(start)();

        AOS_CALL machine.AOS(process_event)(event_processing_request<small_event>{});
        REQUIRE(called_new_operator_type == expected_new_operator_type_for_small_event);

        AOS_CALL machine.AOS(process_event)(event_processing_request<big_event>{});
        REQUIRE(called_new_operator_type == expected_new_operator_type_for_big_event);
    }

    namespace small_event_requirements_1_1
    {
        AOS_TEST_CASE("small_event_requirements<1, 1>")
        {
            AOS_CALL test<1, 1>(new_operator_type::plain, new_operator_type::plain);
        }
    }

    namespace small_event_requirements_1_small
    {
        AOS_TEST_CASE("small_event_requirements<1, small>")
        {
            AOS_CALL test<1, alignof(small_event)>(new_operator_type::plain, new_operator_type::plain);
        }
    }

    namespace small_event_requirements_small_1
    {
        AOS_TEST_CASE("small_event_requirements<small, 1>")
        {
            AOS_CALL test<sizeof(small_event), 1>(new_operator_type::plain, new_operator_type::plain);
        }
    }

    namespace small_event_requirements_small_small
    {
        AOS_TEST_CASE("small_event_requirements<small, small>")
        {
            AOS_CALL test<sizeof(small_event), alignof(small_event)>(new_operator_type::placement, new_operator_type::plain);
        }
    }

    namespace small_event_requirements_big_big
    {
        AOS_TEST_CASE("small_event_requirements<big, big>")
        {
            AOS_CALL test<sizeof(big_event), alignof(big_event)>(new_operator_type::placement, new_operator_type::placement);
        }
    }
}
