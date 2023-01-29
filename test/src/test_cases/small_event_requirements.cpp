//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include "common.hpp"
#include <awesm.hpp>

namespace
{
    struct sm_def;
    using sm_t = awesm::sm<sm_def>;

    auto plain_new_operator_call_count = 0;
    auto placement_new_operator_call_count = 0;

    struct context
    {
    };

    template<class T>
    struct event
    {
        T data;

        static void* operator new(size_t size)
        {
            ++plain_new_operator_call_count;
            return ::operator new(size);
        }

        template<class Storage>
        static void* operator new(size_t size, Storage& storage)
        {
            ++placement_new_operator_call_count;
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
        using conf_type = awesm::state_conf<>;
    };

    template<class Event>
    void process_event(sm_t& sm, context& /*ctx*/, const event_processing_request<Event>& /*event*/);

    using sm_transition_table = awesm::transition_table
    <
        awesm::row<state, event_processing_request<small_event>, awesm::null, process_event<small_event>>,
        awesm::row<state, event_processing_request<big_event>,   awesm::null, process_event<big_event>>
    >;

    struct sm_def
    {
        using conf_type = awesm::sm_conf
        <
            sm_transition_table,
            context,
            awesm::sm_options::small_event_max_size<sizeof(small_event)>,
            awesm::sm_options::small_event_max_align<alignof(small_event)>
        >;
    };

    template<class Event>
    void process_event(sm_t& sm, context& /*ctx*/, const event_processing_request<Event>& /*event*/)
    {
        sm.process_event(Event{});
    }
}

TEST_CASE("small_event_requirements")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();

    plain_new_operator_call_count = 0;
    placement_new_operator_call_count = 0;
    sm.process_event(event_processing_request<small_event>{});
    REQUIRE(plain_new_operator_call_count == 0);
    REQUIRE(placement_new_operator_call_count == 1);

    plain_new_operator_call_count = 0;
    placement_new_operator_call_count = 0;
    sm.process_event(event_processing_request<big_event>{});
    REQUIRE(plain_new_operator_call_count == 1);
    REQUIRE(placement_new_operator_call_count == 0);
}
