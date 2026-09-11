//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_EVENT_ACTION_HPP
#define MAKI_DETAIL_EVENT_ACTION_HPP

#include "call.hpp"
#include "type_set.hpp"
#include "tlu/find_if.hpp"
#include "../action.hpp"
#include <type_traits>
#include <utility>

namespace maki::detail
{

/*
This class holds a callable `action`.
The action must be invoked for any event type in `event_type_set`.
The signature of the action is defined by `sig`.
*/
template<class EventTypeSet, class Action, action_signature Sig>
struct event_action
{
    using event_type_set = EventTypeSet;

    static constexpr action_signature sig = Sig;

    Action action;
};

template<action_signature Sig, class EventTypeSet, class Action>
constexpr auto make_event_action(const Action& action)
{
    return event_action<EventTypeSet, Action, Sig>{action};
}

namespace event_action_traits
{
    template<class Event>
    struct for_event
    {
        template<class EventActionConstant>
        struct has_containing_event_set
        {
            using event_action_type = std::decay_t<decltype(*EventActionConstant::value)>;

            static constexpr bool value = type_set_contains_v
            <
                typename event_action_type::event_type_set,
                Event
            >;
        };
    };
}

} //namespace

#include "aos_sync_begin.hpp"
#include "event_action.inc.hpp"
#include "aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "aos_async_begin.hpp"
#include "event_action.inc.hpp"
#include "aos_end.hpp"
#endif

#endif
