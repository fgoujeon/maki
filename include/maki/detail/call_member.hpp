//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CALL_MEMBER_HPP
#define MAKI_DETAIL_CALL_MEMBER_HPP

#include "type_traits.hpp"
#include "state_traits.hpp"
#include "submachine_fwd.hpp"
#include <functional>
#include <type_traits>
#include <utility>

namespace maki::detail
{

template<class State, class Sm, class Context, class Event, class EventAction, class... EventActions>
void call_event_action
(
    [[maybe_unused]] State& state,
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event,
    [[maybe_unused]] const EventAction& event_action,
    [[maybe_unused]] const EventActions&... event_actions
)
{
    using event_type_filter = typename EventAction::event_type_filter;
    if constexpr(matches_pattern_v<Event, event_type_filter>)
    {
        if constexpr(EventAction::sig == event_action_signature::v)
        {
            std::invoke(event_action.action);
        }
        else if constexpr(EventAction::sig == event_action_signature::m)
        {
            std::invoke(event_action.action, mach);
        }
        else if constexpr(EventAction::sig == event_action_signature::mce)
        {
            std::invoke(event_action.action, mach, ctx, event);
        }
        else if constexpr(EventAction::sig == event_action_signature::c)
        {
            std::invoke(event_action.action, ctx);
        }
        else if constexpr(EventAction::sig == event_action_signature::ce)
        {
            std::invoke(event_action.action, ctx, event);
        }
        else if constexpr(EventAction::sig == event_action_signature::d)
        {
            std::invoke(event_action.action, state);
        }
        else if constexpr(EventAction::sig == event_action_signature::de)
        {
            std::invoke(event_action.action, state, event);
        }
        else if constexpr(EventAction::sig == event_action_signature::e)
        {
            std::invoke(event_action.action, event);
        }
        else
        {
            constexpr auto is_false = sizeof(Sm) == 0;
            static_assert(is_false, "Unsupported event_action_signature value");
        }
    }
    else
    {
        static_assert(sizeof...(EventActions) != 0, "No event action found for this state and event");
        call_event_action(state, mach, ctx, event, event_actions...);
    }
}

template<class State, class Sm, class Context, class Event>
void call_on_entry(State& state, Sm& mach, Context& ctx, const Event& event)
{
    using conf_t = std::decay_t<decltype(State::conf)>;
    using entry_action_tuple_t = std::decay_t<decltype(conf_t::entry_actions)>;
    if constexpr(!tlu::empty_v<entry_action_tuple_t>)
    {
        apply
        (
            state.conf.entry_actions,
            [](auto&&... args)
            {
                call_event_action(std::forward<decltype(args)>(args)...);
            },
            state,
            mach,
            ctx,
            event
        );
    }
}

template<class State, class Sm, class Context, class Event>
void call_on_event(State& state, Sm& mach, Context& ctx, const Event& event)
{
    using conf_t = std::decay_t<decltype(State::conf)>;
    using event_action_tuple_t = std::decay_t<decltype(conf_t::event_actions)>;
    if constexpr(!tlu::empty_v<event_action_tuple_t>)
    {
        apply
        (
            state.conf.event_actions,
            [](auto&&... args)
            {
                call_event_action(std::forward<decltype(args)>(args)...);
            },
            state,
            mach,
            ctx,
            event
        );
    }
}

template<class State, class Sm, class Context, class Event>
void call_on_event
(
    State& state,
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] Context& ctx,
    const Event& event,
    bool& processed
)
{
    if constexpr(state_traits::is_submachine_v<State>)
    {
        state.on_event(event, processed);
    }
    else
    {
        call_on_event(state, mach, ctx, event);
        processed = true;
    }
}

template<class State, class Sm, class Context, class Event>
void call_on_exit(State& state, Sm& mach, Context& ctx, const Event& event)
{
    using conf_t = std::decay_t<decltype(State::conf)>;
    using exit_action_tuple_t = std::decay_t<decltype(conf_t::exit_actions)>;
    if constexpr(!tlu::empty_v<exit_action_tuple_t>)
    {
        apply
        (
            state.conf.exit_actions,
            [](auto&&... args)
            {
                call_event_action(std::forward<decltype(args)>(args)...);
            },
            state,
            mach,
            ctx,
            event
        );
    }
}

template<const auto& Fn, class Sm, class Context, class Event>
auto call_action_or_guard
(
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event
)
{
    using fn_t = std::decay_t<decltype(Fn)>;

    if constexpr(std::is_invocable_v<fn_t, Sm&, Context&, const Event&>)
    {
        return Fn(mach, ctx, event);
    }
    else if constexpr(std::is_invocable_v<fn_t, Context&, const Event&>)
    {
        return Fn(ctx, event);
    }
    else if constexpr(std::is_invocable_v<fn_t, Context&>)
    {
        return Fn(ctx);
    }
    else if constexpr(is_nullary_v<fn_t>)
    {
        return Fn();
    }
    else
    {
        constexpr auto is_false = sizeof(Sm) == 0;
        static_assert(is_false, "No valid signature found for action/guard");
    }
}

} //namespace

#endif
