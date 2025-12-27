//Copyright Florian Goujeon 2021 - 2025.
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

template
<
    auto EventActionPtr,
    class Machine,
    class Context,
    class Event,
    class... ExtraArgs
>
void call_event_action
(
    [[maybe_unused]] Machine& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event,
    [[maybe_unused]] ExtraArgs&&... extra_args
)
{
    call_callable<action_signature, EventActionPtr->sig>
    (
        EventActionPtr->action,
        ctx,
        mach,
        event,
        std::forward<ExtraArgs>(extra_args)...
    );
}

template
<
    class ActionConstantList,
    class Machine,
    class Context,
    class Event,
    class... ExtraArgs
>
void call_matching_event_action
(
    Machine& mach,
    Context& ctx,
    const Event& event,
    ExtraArgs&&... extra_args
)
{
    using matching_action_constant_t = tlu::find_if_t
    <
        ActionConstantList,
        event_action_traits::for_event<Event>::template has_containing_event_set
    >;

    call_event_action<matching_action_constant_t::value>
    (
        mach,
        ctx,
        event,
        std::forward<ExtraArgs>(extra_args)...
    );
}

} //namespace

#endif
