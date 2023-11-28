//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_EVENT_ACTION_HPP
#define MAKI_DETAIL_EVENT_ACTION_HPP

#define MAKI_DETAIL_EVENT_ACTION_SIGNATURES \
    X(v) \
    X(m) \
    X(me) \
    X(c) \
    X(ce) \
    X(d) \
    X(de) \
    X(e)

namespace maki::detail
{

/*
v means void
m means machine
c means context
d means data
e means event
*/
enum class event_action_signature
{
#define X(name) name, /*NOLINT(cppcoreguidelines-macro-usage)*/
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef X
};

template<class EventFilter, class Action, event_action_signature Sig>
struct event_action
{
    using event_type_filter = EventFilter;
    static constexpr auto sig = Sig;
    Action action;
};

} //namespace

#endif
