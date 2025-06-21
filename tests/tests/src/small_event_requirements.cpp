//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include "common.hpp"
#include <maki.hpp>

namespace small_event_requirements_ns
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
    inline constexpr auto process_event = maki::action_m([](auto& machine)
    {
        machine.process_event(Event{});
    });

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,  state)
        (state, maki::null, maki::event<event_processing_request<small_event>>, process_event<small_event>)
        (state, maki::null, maki::event<event_processing_request<big_event>>, process_event<big_event>)
    ;

    template<size_t SmallEventMaxSize, size_t SmallEventMaxAlign>
    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
        .small_event_max_size(SmallEventMaxSize)
        .small_event_max_align(SmallEventMaxAlign)
    ;

    template<size_t SmallEventMaxSize, size_t SmallEventMaxAlign>
    using machine_t = maki::machine<machine_conf<SmallEventMaxSize, SmallEventMaxAlign>>;

    template<size_t SmallEventMaxSize, size_t SmallEventMaxAlign>
    void test
    (
        const new_operator_type expected_new_operator_type_for_small_event,
        const new_operator_type expected_new_operator_type_for_big_event
    )
    {
        auto machine = machine_t<SmallEventMaxSize, SmallEventMaxAlign>{};

        machine.start();

        machine.process_event(event_processing_request<small_event>{});
        REQUIRE(called_new_operator_type == expected_new_operator_type_for_small_event);

        machine.process_event(event_processing_request<big_event>{});
        REQUIRE(called_new_operator_type == expected_new_operator_type_for_big_event);
    }
}

TEST_CASE("small_event_requirements<1, 1>")
{
    using namespace small_event_requirements_ns;
    test<1, 1>(new_operator_type::plain, new_operator_type::plain);
}

TEST_CASE("small_event_requirements<1, small>")
{
    using namespace small_event_requirements_ns;
    test<1, alignof(small_event)>(new_operator_type::plain, new_operator_type::plain);
}

TEST_CASE("small_event_requirements<small, 1>")
{
    using namespace small_event_requirements_ns;
    test<sizeof(small_event), 1>(new_operator_type::plain, new_operator_type::plain);
}

TEST_CASE("small_event_requirements<small, small>")
{
    using namespace small_event_requirements_ns;
    test<sizeof(small_event), alignof(small_event)>(new_operator_type::placement, new_operator_type::plain);
}

TEST_CASE("small_event_requirements<big, big>")
{
    using namespace small_event_requirements_ns;
    test<sizeof(big_event), alignof(big_event)>(new_operator_type::placement, new_operator_type::placement);
}
