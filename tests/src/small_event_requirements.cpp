//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include "common.hpp"
#include <awesm.hpp>

namespace
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

    struct state
    {
        using conf = awesm::state_conf;
    };

    template<class Event>
    inline constexpr auto process_event = [](auto& sm, context& /*ctx*/, const auto& /*event*/)
    {
        sm.process_event(Event{});
    };

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<state, event_processing_request<small_event>, awesm::null, process_event<small_event>>
            .add<state, event_processing_request<big_event>,   awesm::null, process_event<big_event>>
        ;
    }

    template<size_t SmallEventMaxSize, size_t SmallEventMaxAlign>
    struct sm_def
    {
        using conf = typename awesm::sm_conf
            ::transition_table<sm_transition_table>
            ::context<context>
            ::small_event_max_size<SmallEventMaxSize>
            ::template small_event_max_align<SmallEventMaxAlign>
        ;
    };

    template<size_t SmallEventMaxSize, size_t SmallEventMaxAlign>
    using sm_t = awesm::sm<sm_def<SmallEventMaxSize, SmallEventMaxAlign>>;

    template<size_t SmallEventMaxSize, size_t SmallEventMaxAlign>
    void test
    (
        const new_operator_type expected_new_operator_type_for_small_event,
        const new_operator_type expected_new_operator_type_for_big_event
    )
    {
        auto ctx = context{};
        auto sm = sm_t<SmallEventMaxSize, SmallEventMaxAlign>{ctx};

        sm.start();

        sm.process_event(event_processing_request<small_event>{});
        REQUIRE(called_new_operator_type == expected_new_operator_type_for_small_event);

        sm.process_event(event_processing_request<big_event>{});
        REQUIRE(called_new_operator_type == expected_new_operator_type_for_big_event);
    }
}

TEST_CASE("small_event_requirements<1, 1>")
{
    test<1, 1>(new_operator_type::plain, new_operator_type::plain);
}

TEST_CASE("small_event_requirements<1, small>")
{
    test<1, alignof(small_event)>(new_operator_type::plain, new_operator_type::plain);
}

TEST_CASE("small_event_requirements<small, 1>")
{
    test<sizeof(small_event), 1>(new_operator_type::plain, new_operator_type::plain);
}

TEST_CASE("small_event_requirements<small, small>")
{
    test<sizeof(small_event), alignof(small_event)>(new_operator_type::placement, new_operator_type::plain);
}

TEST_CASE("small_event_requirements<big, big>")
{
    test<sizeof(big_event), alignof(big_event)>(new_operator_type::placement, new_operator_type::placement);
}
