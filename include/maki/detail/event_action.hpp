//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_EVENT_ACTION_HPP
#define MAKI_DETAIL_EVENT_ACTION_HPP

#define MAKI_DETAIL_EVENT_ACTION_SIGNATURES \
    MAKI_DETAIL_X(v) \
    MAKI_DETAIL_X(m) \
    MAKI_DETAIL_X(me) \
    MAKI_DETAIL_X(c) \
    MAKI_DETAIL_X(ce) \
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

} //namespace

#endif
