//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_EVENT_ACTION_HPP
#define MAKI_DETAIL_EVENT_ACTION_HPP

#include "tlu.hpp"
#include "tuple.hpp"
#include "../filters.hpp"
#include <functional>

#define MAKI_DETAIL_EVENT_ACTION_SIGNATURES \
    MAKI_DETAIL_X(v) \
    MAKI_DETAIL_X(c) \
    MAKI_DETAIL_X(cm) \
    MAKI_DETAIL_X(cme) \
    MAKI_DETAIL_X(ce) \
    MAKI_DETAIL_X(m) \
    MAKI_DETAIL_X(me) \
    MAKI_DETAIL_X(e)

namespace maki::detail
{

/*
v means void
m means machine
c means context
e means event
*/
enum class event_action_signature
{
#define MAKI_DETAIL_X(name) name, /*NOLINT(cppcoreguidelines-macro-usage)*/
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X
};

template<class EventFilter, class Action, event_action_signature Sig>
struct event_action
{
    using event_filter_type = EventFilter;

    static constexpr auto sig = Sig;
    EventFilter event_filter;
    Action action;
};

template<event_action_signature Sig, class EventFilter, class Action>
constexpr auto make_event_action(const EventFilter& event_filter, const Action& action)
{
    return event_action<EventFilter, Action, Sig>{event_filter, action};
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

template<auto EventActionPtr, class Machine, class Context, class Event>
void call_event_action
(
    [[maybe_unused]] Machine& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event
)
{
    if constexpr(EventActionPtr->sig == event_action_signature::v)
    {
        std::invoke(EventActionPtr->action);
    }
    else if constexpr(EventActionPtr->sig == event_action_signature::c)
    {
        std::invoke(EventActionPtr->action, ctx);
    }
    else if constexpr(EventActionPtr->sig == event_action_signature::cm)
    {
        std::invoke(EventActionPtr->action, ctx, mach);
    }
    else if constexpr(EventActionPtr->sig == event_action_signature::cme)
    {
        std::invoke(EventActionPtr->action, ctx, mach, event);
    }
    else if constexpr(EventActionPtr->sig == event_action_signature::ce)
    {
        std::invoke(EventActionPtr->action, ctx, event);
    }
    else if constexpr(EventActionPtr->sig == event_action_signature::m)
    {
        std::invoke(EventActionPtr->action, mach);
    }
    else if constexpr(EventActionPtr->sig == event_action_signature::me)
    {
        std::invoke(EventActionPtr->action, mach, event);
    }
    else if constexpr(EventActionPtr->sig == event_action_signature::e)
    {
        std::invoke(EventActionPtr->action, event);
    }
    else
    {
        constexpr auto is_false = sizeof(Machine) == 0;
        static_assert(is_false, "Unsupported event_action_signature value");
    }
}

template
<
    class ActionConstantList,
    class Machine,
    class Context,
    class Event
>
void call_matching_event_action
(
    Machine& mach,
    Context& ctx,
    const Event& event
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
        event
    );
}

} //namespace

#endif
