//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_EVENT_ACTION_HPP
#define MAKI_DETAIL_EVENT_ACTION_HPP

#include "call.hpp"
#include "tlu/get.hpp"
#include "../filter.hpp"
#include "../action.hpp"
#include <utility>

namespace maki::detail
{

template<class EventFilterPredicate, class Action, action_signature Sig>
struct event_action
{
    using event_filter_type = filter<EventFilterPredicate>;

    static constexpr auto sig = Sig;
    event_filter_type event_filter;
    Action action;
};

template<action_signature Sig, class EventFilterPredicate, class Action>
constexpr auto make_event_action(const filter<EventFilterPredicate>& event_filter, const Action& action)
{
    return event_action<EventFilterPredicate, Action, Sig>{event_filter, action};
}

namespace event_action_traits
{
    template<class Event>
    struct for_event
    {
        template<class EventActionConstant>
        struct has_matching_event_filter
        {
            static constexpr auto value = matches_filter(type<Event>, EventActionConstant::value->event_filter);
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
    constexpr auto matching_action_index = tlu::find_if_v
    <
        ActionConstantList,
        event_action_traits::for_event<Event>::template has_matching_event_filter
    >;

    call_event_action<tlu::get_t<ActionConstantList, matching_action_index>::value>
    (
        mach,
        ctx,
        event,
        std::forward<ExtraArgs>(extra_args)...
    );
}

} //namespace

#endif
