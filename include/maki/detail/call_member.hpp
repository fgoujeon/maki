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
#include "overload_priority.hpp"
#include <functional>
#include <type_traits>
#include <utility>

namespace maki::detail
{

#define MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(name) /*NOLINT*/ \
    template<class State, class... Args> \
    constexpr auto has_##name##_impl(overload_priority::high /*unused*/) -> \
        decltype(std::declval<State>().name(std::declval<Args>()...), bool()) \
    { \
        return true; \
    } \
 \
    template<class State, class... Args> \
    constexpr bool has_##name##_impl(overload_priority::low /*unused*/) \
    { \
        return false; \
    } \
 \
    template<class State, class... Args> \
    constexpr bool has_##name() \
    { \
        return has_##name##_impl<State, Args...>(overload_priority::probe); \
    }

MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_entry)
MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_event)
MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_event_ce)
MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_event_mce)
MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION(on_exit)

#undef MAKI_DETAIL_GENERATE_HAS_MEMBER_FUNCTION

template<class State, class Sm, class Context, class Event, class EntryAction, class... EntryActions>
void call_entry_action_2
(
    [[maybe_unused]] State& state,
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event,
    [[maybe_unused]] const EntryAction& entry_action,
    [[maybe_unused]] const EntryActions&... entry_actions
)
{
    using event_type_filter = typename EntryAction::event_type_filter;
    if constexpr(matches_pattern_v<Event, event_type_filter>)
    {
        if constexpr(EntryAction::sig == event_action_signature::v)
        {
            std::invoke(entry_action.action);
        }
        else if constexpr(EntryAction::sig == event_action_signature::m)
        {
            std::invoke(entry_action.action, mach);
        }
        else if constexpr(EntryAction::sig == event_action_signature::c)
        {
            std::invoke(entry_action.action, ctx);
        }
        else if constexpr(EntryAction::sig == event_action_signature::ce)
        {
            std::invoke(entry_action.action, ctx, event);
        }
        else if constexpr(EntryAction::sig == event_action_signature::d)
        {
            std::invoke(entry_action.action, state);
        }
        else if constexpr(EntryAction::sig == event_action_signature::de)
        {
            std::invoke(entry_action.action, state, event);
        }
        else if constexpr(EntryAction::sig == event_action_signature::e)
        {
            std::invoke(entry_action.action, event);
        }
        else
        {
            constexpr auto is_false = sizeof(Sm) == 0;
            static_assert(is_false, "Unsupported event_action_signature value");
        }
    }
    else
    {
        static_assert(sizeof...(EntryActions) != 0, "No entry action found for this state and event");
        call_entry_action_2(state, mach, ctx, event, entry_actions...);
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
                call_entry_action_2(std::forward<decltype(args)>(args)...);
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
    if constexpr(has_on_event<State&, const Event&>())
    {
        state.on_event(event);
    }
    else if constexpr(has_on_event_ce<State&, Context&, const Event&>())
    {
        state.on_event_ce(ctx, event);
    }
    else if constexpr(has_on_event_mce<State&, Sm&, Context&, const Event&>())
    {
        state.on_event_mce(mach, ctx, event);
    }
    else
    {
        constexpr auto is_false = sizeof(Sm) == 0;
        static_assert(is_false, "No valid on_event() signature found in state");
    }
}

template<class State, class Sm, class Context, class Event>
void call_on_event(State& state, Sm& /*mach*/, Context& /*ctx*/, const Event& event, bool& processed)
{
    if constexpr(state_traits::is_submachine_v<State>)
    {
        state.on_event(event, processed);
    }
    else
    {
        state.on_event(event);
        processed = true;
    }
}

template<class State, class Sm, class Context, class Event, class EntryAction, class... EntryActions>
void call_exit_action_2
(
    [[maybe_unused]] State& state,
    [[maybe_unused]] Sm& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event,
    [[maybe_unused]] const EntryAction& exit_action,
    [[maybe_unused]] const EntryActions&... exit_actions
)
{
    using event_type_filter = typename EntryAction::event_type_filter;
    if constexpr(matches_pattern_v<Event, event_type_filter>)
    {
        if constexpr(EntryAction::sig == event_action_signature::v)
        {
            std::invoke(exit_action.action);
        }
        else if constexpr(EntryAction::sig == event_action_signature::m)
        {
            std::invoke(exit_action.action, mach);
        }
        else if constexpr(EntryAction::sig == event_action_signature::c)
        {
            std::invoke(exit_action.action, ctx);
        }
        else if constexpr(EntryAction::sig == event_action_signature::ce)
        {
            std::invoke(exit_action.action, ctx, event);
        }
        else if constexpr(EntryAction::sig == event_action_signature::d)
        {
            std::invoke(exit_action.action, state);
        }
        else if constexpr(EntryAction::sig == event_action_signature::de)
        {
            std::invoke(exit_action.action, state, event);
        }
        else if constexpr(EntryAction::sig == event_action_signature::e)
        {
            std::invoke(exit_action.action, event);
        }
        else
        {
            constexpr auto is_false = sizeof(Sm) == 0;
            static_assert(is_false, "Unsupported event_action_signature value");
        }
    }
    else
    {
        static_assert(sizeof...(EntryActions) != 0, "No exit action found for this state and event");
        call_exit_action_2(state, mach, ctx, event, exit_actions...);
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
                call_exit_action_2(std::forward<decltype(args)>(args)...);
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
