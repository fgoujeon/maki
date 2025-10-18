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
#include "event.hpp"
#include "state_set.hpp"
#include "states.hpp"
#include "ini.hpp"
#include "fin.hpp"
#include "null.hpp"
#include "detail/tlu/left_fold.hpp"
#include "detail/type_set.hpp"
#include "detail/friendly_impl.hpp"
#include "detail/state_mold_fwd.hpp"
#include "detail/tuple.hpp"

namespace maki
{

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
    (maki::ini,  off)
    (off,        on,     maki::event<button_press>, turn_light_on, has_enough_power)
    (on,         off,    maki::event<button_press>, turn_light_off)
;
```
*/
template<class Impl = IMPLEMENTATION_DETAIL>
class transition_table;
#else
template<class Impl = detail::tuple<>>
class transition_table;
#endif

namespace detail
{
    template
    <
        class SourceStateMold,
        class TargetStateMold,
        class Event,
        action_signature ActionSignature,
        class ActionCallable,
        guard_signature GuardSignature,
        class GuardCallable
    >
    struct transition
    {
        using event_type = Event;

        SourceStateMold source_state_mold;
        TargetStateMold target_state_mold;
        Event evt;
        action<ActionSignature, ActionCallable> act;
        guard<GuardSignature, GuardCallable> grd;

        [[nodiscard]]
        static constexpr bool can_process_completion_event()
        {
            return is_null_v<Event>;
        }
    };

    template
    <
        class SourceStateMold,
        class TargetStateMold,
        class Event,
        action_signature ActionSignature,
        class ActionCallable,
        guard_signature GuardSignature,
        class GuardCallable
    >
    transition
    (
        SourceStateMold,
        TargetStateMold,
        Event,
        action<ActionSignature, ActionCallable>,
        guard<GuardSignature, GuardCallable>
    ) -> transition
    <
        SourceStateMold,
        TargetStateMold,
        Event,
        ActionSignature,
        ActionCallable,
        GuardSignature,
        GuardCallable
    >;

    template<class TransitionEvent>
    struct transition_event_event_type_set;

    template<class Event>
    struct transition_event_event_type_set<event_t<Event>>
    {
        using type = type_set_item<Event>;
    };

    template<class... Events>
    struct transition_event_event_type_set<event_set<Events...>>
    {
        using type = impl_of_t<event_set<Events...>>;
    };

    template<>
    struct transition_event_event_type_set<null_t>
    {
        using type = empty_type_set_t;
    };

    template<class TransitionEvent>
    using transition_event_event_type_set_t = typename transition_event_event_type_set<TransitionEvent>::type;

    template<class Transition>
    using transition_event_type_set_t = transition_event_event_type_set_t<typename Transition::event_type>;

    template<class... Transitions>
    constexpr auto make_transition_table(const tuple<Transitions...>& transitions)
    {
        return transition_table<tuple<Transitions...>>{transitions};
    }

    template<class T>
    constexpr decltype(auto) store_state_mold(T&& obj)
    {
        return std::forward<T>(obj);
    }

    //Store a pointer in this case
    constexpr auto store_state_mold(ini_t /*ini*/)
    {
        return &detail::state_molds::null;
    }

    //Store a pointer in this case
    constexpr auto store_state_mold(fin_t /*fin*/)
    {
        return &detail::state_molds::fin;
    }

    //Store a pointer in this case
    template<class OptionSet>
    constexpr auto store_state_mold(const state_mold<OptionSet>& mold)
    {
        return &mold;
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

    @param source_state_mold the mold of the active state (or states, plural, if it's a @ref state-set "state set") from which the transition can occur
    @param target_state_mold the mold of the state that becomes active after the transition occurs
    @param evt the event type (or types, plural, if it's an @ref event-set "event type set") that can cause the transition to occur
    @param action the `maki::action` invoked when the transition occurs, or `maki::null`
    @param guard the `maki::guard` that must return `true` for the transition to occur, or `maki::null`
    */
    template
    <
        class Source,
        class Target,
        class Event = null_t,
        class ActionOrNull = null_t,
        class GuardOrNull = null_t
    >
    constexpr auto operator()
    (
        const Source& source_state_mold,
        const Target& target_state_mold,
        const Event& evt = null,
        const ActionOrNull& action = null,
        const GuardOrNull& guard = null
    )
    {
        //Check source
        if constexpr(Impl::size == 0)
        {
            static_assert
            (
                detail::is_ini_v<Source>,
                "Source (1st argument) of first transition must be `maki::ini`. (Note: Composite state regions without initial pseudostate are not implemented yet.)"
            );
        }
        else
        {
            static_assert
            (
                detail::is_state_mold_v<Source> || detail::is_state_set_v<Source>,
                "Source (1st argument) must be an instance of `maki::state_mold` or an instance of `maki::state_set`"
            );
        }

        //Check target
        if constexpr(detail::is_ini_v<Source>)
        {
            static_assert
            (
                detail::is_state_mold_v<Target>,
                "Target (2nd argument) of transition from initial pseudostate must be an instance of `maki::state_mold`."
            );
        }
        else
        {
            static_assert
            (
                detail::is_state_mold_v<Target> || detail::is_null_v<Target> || detail::is_fin_v<Target>,
                "Target (2nd argument) must be an instance of `maki::state_mold`, `maki::null` or `maki::fin`."
            );
        }

        //Check event
        if constexpr(detail::is_ini_v<Source>)
        {
            static_assert
            (
                detail::is_null_v<Event>,
                "Event (3rd argument) of transition from initial pseudostate must be `maki::null`"
            );
        }
        else
        {
            static_assert
            (
                detail::is_event_v<Event> || detail::is_event_set_v<Event> || detail::is_null_v<Event>,
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
        if constexpr(detail::is_ini_v<Source>)
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
            impl_.append
            (
                detail::transition
                {
                    detail::store_state_mold(source_state_mold),
                    detail::store_state_mold(target_state_mold),
                    evt,
                    detail::to_action(action),
                    detail::to_guard(guard)
                }
            )
        );
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    using impl_type = Impl;

#ifndef MAKI_DETAIL_DOXYGEN
    template<class... Transitions>
    friend constexpr auto detail::make_transition_table(const detail::tuple<Transitions...>&);
#endif

    constexpr explicit transition_table(const Impl& impl):
        impl_{impl}
    {
    }

    impl_type impl_;
};

namespace detail
{
    template<class EventTypeSet, class Transition>
    using transition_table_event_type_set_fold_operation_t = type_set_union_t
    <
        EventTypeSet,
        transition_event_type_set_t<Transition>
    >;

    template<class TransitionTable>
    using transition_table_event_type_set_t = tlu::left_fold_t
    <
        impl_of_t<TransitionTable>,
        transition_table_event_type_set_fold_operation_t,
        empty_type_set_t
    >;
}

} //namespace

#endif
