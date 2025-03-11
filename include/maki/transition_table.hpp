//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::transition and maki::transition_table struct templates
*/

#ifndef MAKI_TRANSITION_TABLE_HPP
#define MAKI_TRANSITION_TABLE_HPP

#include "action.hpp"
#include "guard.hpp"
#include "event_set.hpp"
#include "state_set.hpp"
#include "states.hpp"
#include "init.hpp"
#include "final.hpp"
#include "catch.hpp"
#include "null.hpp"
#include "detail/impl.hpp"
#include "detail/state_builder_fwd.hpp"
#include "detail/tuple.hpp"

namespace maki
{

namespace detail
{
    template<class... Transitions>
    struct transition_table_impl
    {
        static constexpr auto size = sizeof...(Transitions);
        tuple<Transitions...> transitions;
    };
}

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief Represents a transition table.

A transition table lists all the possible transitions from a state (the source
state) to another (the target state) in a region of a state machine.

To define a transition table, you have to instantiate an empty
`maki::transition_table` and call
`maki::transition_table::operator()()` for each transition, like so:

```cpp
constexpr auto transition_table = maki::transition_table{}
    //source,    target, event,                     action,        guard
    (maki::init, off)
    (off,        on,     maki::event<button_press>, turn_light_on, has_enough_power)
    (on,         off,    maki::event<button_press>, turn_light_off)
;
```
*/
template<class Impl = IMPLEMENTATION_DETAIL>
class transition_table;
#else
template<class Impl = detail::transition_table_impl<>>
class transition_table;
#endif

namespace detail
{
    template
    <
        class SourceStateBuilder,
        class TargetStateBuilder,
        class EventSet,
        action_signature ActionSignature,
        class ActionCallable,
        guard_signature GuardSignature,
        class GuardCallable
    >
    struct transition
    {
        SourceStateBuilder source_state_builder;
        TargetStateBuilder target_state_builder;
        EventSet evt_set;
        action<ActionSignature, ActionCallable> act;
        guard<GuardSignature, GuardCallable> grd;
    };

    template
    <
        class SourceStateBuilder,
        class TargetStateBuilder,
        class EventSet,
        action_signature ActionSignature,
        class ActionCallable,
        guard_signature GuardSignature,
        class GuardCallable
    >
    transition
    (
        SourceStateBuilder,
        TargetStateBuilder,
        EventSet,
        action<ActionSignature, ActionCallable>,
        guard<GuardSignature, GuardCallable>
    ) -> transition
    <
        SourceStateBuilder,
        TargetStateBuilder,
        EventSet,
        ActionSignature,
        ActionCallable,
        GuardSignature,
        GuardCallable
    >;

    template<class... Transitions>
    constexpr auto make_transition_table(const tuple<Transitions...>& transitions)
    {
        using impl_t = transition_table_impl<Transitions...>;
        return transition_table<impl_t>{impl_t{transitions}};
    }

    template<class Impl>
    constexpr const auto& rows(const transition_table<Impl>& table)
    {
        return impl_of(table).transitions;
    }

    template<class T>
    constexpr decltype(auto) store_state_builder(T&& obj)
    {
        return std::forward<T>(obj);
    }

    //Store a pointer in this case
    constexpr auto store_state_builder(init_t /*ignored*/)
    {
        return &detail::state_builders::null;
    }

    //Store a pointer in this case
    constexpr auto store_state_builder(final_t /*ignored*/)
    {
        return &detail::state_builders::final;
    }

    //Store a pointer in this case
    constexpr auto store_state_builder(catch_t /*ignored*/)
    {
        return &catch_;
    }

    //Store a pointer in this case
    template<class OptionSet>
    constexpr auto store_state_builder(const state_builder<OptionSet>& builder)
    {
        return &builder;
    }
}

template<class Impl>
class transition_table
{
public:
    /**
    @brief Default constructor
    */
    constexpr transition_table() = default;

    /**
    @brief Copy constructor
    */
    constexpr transition_table(const transition_table&) = default;

    /**
    @brief Deleted move constructor
    */
    transition_table(transition_table&&) = delete;

    ~transition_table() = default;

    /**
    @brief Deleted assignment operator
    */
    transition_table& operator=(const transition_table&) = delete;

    /**
    @brief Deleted move operator
    */
    transition_table& operator=(transition_table&&) = delete;

    /**
    @brief Creates a new `transition_table` with an additional transition.

    @param source_state_builder the builder of the active state (or states, plural, if it's a @ref state-set "state set") from which the transition can occur
    @param target_state_builder the builder of the state that becomes active after the transition occurs
    @param evt_set the event type (or types, plural, if it's an @ref event-set "event type set") that can cause the transition to occur
    @param action the `maki::action` invoked when the transition occurs, or `maki::null`
    @param guard the `maki::guard` that must return `true` for the transition to occur, or `maki::null`
    */
    template
    <
        class Source,
        class Target,
        class EventSet = null_t,
        class ActionOrNull = null_t,
        class GuardOrNull = null_t
    >
    constexpr auto operator()
    (
        const Source& source_state_builder,
        const Target& target_state_builder,
        const EventSet& evt_set = null,
        const ActionOrNull& action = null,
        const GuardOrNull& guard = null
    )
    {
        //Check source
        if constexpr(Impl::size == 0)
        {
            static_assert
            (
                detail::is_init_v<Source>,
                "Source (1st argument) of first transition must be `maki::init`. (Note: Composite state regions without initial pseudostate are not implemented yet.)"
            );
        }
        else
        {
            static_assert
            (
                detail::is_state_builder_v<Source> || detail::is_state_set_v<Source> || detail::is_catch_v<Source>,
                "Source (1st argument) must be an instance of `maki::state_builder`, an instance of `maki::state_set`, or `maki::catch_`"
            );
        }

        //Check target
        if constexpr(detail::is_init_v<Source>)
        {
            static_assert
            (
                detail::is_state_builder_v<Target>,
                "Target (2nd argument) of transition from initial pseudostate must be an instance of `maki::state_builder`."
            );
        }
        else
        {
            static_assert
            (
                detail::is_state_builder_v<Target> || detail::is_null_v<Target> || detail::is_final_v<Target>,
                "Target (2nd argument) must be an instance of `maki::state_builder`, `maki::null` or `maki::final`."
            );
        }

        //Check event
        if constexpr(detail::is_init_v<Source>)
        {
            static_assert
            (
                detail::is_null_v<EventSet>,
                "Event (3rd argument) of transition from initial pseudostate must be `maki::null`"
            );
        }
        else
        {
            static_assert
            (
                detail::is_event_v<EventSet> || detail::is_event_set_v<EventSet> || detail::is_null_v<EventSet>,
                "Event (3rd argument) must be an instance of `maki::event_t`, an instance of `maki::event_set`, or `maki::null`"
            );
        }

        //Check action
        static_assert
        (
            detail::is_action_v<ActionOrNull> || detail::is_null_v<ActionOrNull>,
            "Action (4th argument) must be an instance of `maki::action` or `maki::null`."
        );

        //Check guard
        if constexpr(detail::is_init_v<Source>)
        {
            static_assert
            (
                detail::is_null_v<GuardOrNull>,
                "Guard (5th argument) of transition from initial pseudostate must be `maki::null`."
            );
        }
        else
        {
            static_assert
            (
                detail::is_guard_v<GuardOrNull> || detail::is_null_v<GuardOrNull>,
                "Guard (5th argument) must be an instance of `maki::guard` or `maki::null`."
            );
        }

        return detail::make_transition_table
        (
            tuple_append
            (
                impl_.transitions,
                detail::transition
                {
                    detail::store_state_builder(source_state_builder),
                    detail::store_state_builder(target_state_builder),
                    evt_set,
                    detail::to_action(action),
                    detail::to_guard(guard)
                }
            )
        );
    }

private:
    using impl_type = Impl;

#ifndef MAKI_DETAIL_DOXYGEN
    template<class... Transitions>
    friend constexpr auto detail::make_transition_table(const detail::tuple<Transitions...>&);
#endif

    constexpr explicit transition_table(const Impl& impl):
        impl_{impl}
    {
    }

    MAKI_DETAIL_FRIENDLY_IMPL
};

} //namespace

#endif
