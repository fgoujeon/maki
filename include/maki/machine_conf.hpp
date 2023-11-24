//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine_conf struct template
*/

#ifndef MAKI_MACHINE_CONF_HPP
#define MAKI_MACHINE_CONF_HPP

#include "transition_table.hpp"
#include "type_patterns.hpp"
#include "type_list.hpp"
#include "type.hpp"
#include "detail/event_action.hpp"
#include "detail/tuple.hpp"
#include "detail/tlu.hpp"

namespace maki
{

/**
@brief @ref machine configuration
*/
template
<
    class ContextTypeHolder = type<void>,
    class EntryActionTuple = detail::tuple<>,
    class OnEventTypeList = type_list<>,
    class TransitionTableTypeList = type_list<>
>
struct machine_conf
{
    using context_type = typename ContextTypeHolder::type;

    /**
    @brief Specifies whether the constructor of @ref machine must call @ref machine::start().
    */
    bool auto_start = true; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies the context type.
    */
    ContextTypeHolder context; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `after_state_transition()` member function after any external state
    transition.

    The following expression must be valid, for every possible template argument
    list:
    @code
    machine_def.after_state_transition
    <
        region_path,
        source_state_type,
        event_type,
        target_state_type
    >(event);
    @endcode

    This hook can be useful for logging state transitions, for example.

    Example:
    @code
    struct machine_def
    {
        using conf = maki::machine_conf
            .enable_after_state_transition()
            //...
        ;

        template<const auto& RegionPath, class SourceState, class Event, class TargetState>
        void after_state_transition(const Event& event)
        {
            //...
        }

        //...
    };
    @endcode
    */
    bool has_after_state_transition = false; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `before_state_transition()` member function before any external state
    transition.

    The following expression must be valid, for every possible template argument
    list:
    @code
    machine_def.before_state_transition
    <
        region_path,
        source_state_type,
        event_type,
        target_state_type
    >(event);
    @endcode

    This hook can be useful for logging state transitions, for example.

    Example:
    @code
    struct machine_def
    {
        using conf = maki::machine_conf
            .enable_before_state_transition()
            //...
        ;

        template<const auto& RegionPath, class SourceState, class Event, class TargetState>
        void before_state_transition(const Event& event)
        {
            //...
        }

        //...
    };
    @endcode
    */
    bool has_before_state_transition = false; //NOLINT(misc-non-private-member-variables-in-classes)

    EntryActionTuple entry_actions; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a compatible, user-provided
    `on_event()` member function, provided this function exists, whenever it is
    about to process an event. Run-to-completion guarantee applies.

    In the following example, `on_event()` will only be called for
    `event_type_0` and `event_type_1`:
    @code
    struct machine_def
    {
        static constexpr auto conf = default_machine_conf
            .enable_on_event_auto()
            //...
        ;

        void on_event(const event_type_0& event)
        {
            //...
        }

        template<class Sm>
        void on_event(Sm& fsm, const event_type_1& event)
        {
            //...
        }

        //...
    };
    @endcode
    */
    bool has_on_event_auto = false; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies the event types (and type filters with which event types
    match) for which @ref machine must call a user-provided `on_event()` member
    function whenever it is about to process an event. Run-to-completion
    guarantee applies.

    One of these expressions must be valid, for every event type that matches
    a type or type filter of the list:
    @code
    machine_def.on_event(fsm, event);
    machine_def.on_event(event);
    machine_def.on_event();
    @endcode

    This hook can be useful when there are certain event types that you always
    want to process the same way, whatever the active state(s) of the state
    machine.

    Example:
    @code
    struct machine_def
    {
        static constexpr auto conf = default_machine_conf
            .enable_on_event_for<event_type_0, event_type_1>()
            //...
        ;

        void on_event(const event_type_0& event)
        {
            //...
        }

        template<class Sm>
        void on_event(Sm& fsm, const event_type_1& event)
        {
            //...
        }

        //...
    };
    @endcode

    If manually listing all the event type you're insterested in is too
    inconvenient, you can use @ref has_on_event_auto.
    */
    OnEventTypeList has_on_event_for; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `on_exception()` member function whenever it catches an exception.

    The following expression must be valid:
    @code
    machine_def.on_exception(std::current_exception());
    @endcode

    If this option isn't set, @ref machine will send itself an @ref
    events::exception event, like so:
    @code
    process_event(events::exception{std::current_exception()});
    @endcode

    Example:
    @code
    struct machine_def
    {
        static constexpr auto conf = default_machine_conf
            .enable_on_exception()
            //...
        ;

        void on_exception(const std::exception_ptr& eptr)
        {
            //...
        }

        //...
    };
    @endcode
    */
    bool has_on_exception = false; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided `on_exit()`
    member function whenever it stops.

    One of these expressions must be valid, for every possible event type:
    @code
    machine_def.on_exit(fsm, event);
    machine_def.on_exit(event);
    machine_def.on_exit();
    @endcode

    Example:
    @code
    struct machine_def
    {
        static constexpr auto conf = default_machine_conf
            .enable_on_exit()
            //...
        ;

        void on_exit(const event_type_0& event)
        {
            //...
        }

        template<class Sm>
        void on_exit(Sm& fsm, const event_type_1& event)
        {
            //...
        }

        //For all other event types
        void on_exit()
        {
            //...
        }

        //...
    };
    @endcode
    */
    bool has_on_exit = false; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `on_unprocessed()` member function whenever a call to @ref
    machine::process_event() doesn't lead to any state transition or call to any
    `on_event()` function.

    The said member function must have the following form:
    @code
    void on_unprocessed(const event_type& event);
    @endcode

    State machine definitions typically define several overloads of this
    function, for all events of interest.

    Example:
    @code
    struct machine_def
    {
        static constexpr auto conf = default_machine_conf
            .enable_on_unprocessed()
            //...
        ;

        void on_unprocessed(const some_event_type& event)
        {
            //...
        }

        void on_unprocessed(const some_other_event_type& event)
        {
            //...
        }

        //Ignore all other event types
        template<class Event>
        void on_unprocessed(const Event&)
        {
            //nothing
        }

        //...
    };
    @endcode
    */
    bool has_on_unprocessed = false; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether @ref machine must call a user-provided
    `pretty_name()` static member function to get the pretty name of the state
    machine.

    See `maki::pretty_name`.

    Example:
    @code
    struct machine_def
    {
        static constexpr auto conf = default_machine_conf
            .enable_pretty_name()
            //...
        ;

        static auto pretty_name()
        {
            return "Main FSM";
        }

        //...
    };
    @endcode
    */
    bool has_pretty_name = false; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Specifies whether run-to-completion is enabled.

    Disabling run-to-completion makes the state machine much faster, but you
    have to make sure you <b>never</b> call @ref machine::process_event()
    recursively.

    Disable it at your own risk!
    */
    bool run_to_completion = true; //NOLINT(misc-non-private-member-variables-in-classes)

    /**
    @brief Maximum object alignment requirement for the run-to-completion event
    queue to enable small object optimization (and thus avoid an extra memory
    allocation).
    */
    std::size_t small_event_max_align = 8; //NOLINT(misc-non-private-member-variables-in-classes, cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    /**
    @brief Maximum object size for the run-to-completion event queue to enable
    small object optimization (and thus avoid an extra memory allocation).
    */
    std::size_t small_event_max_size = 16; //NOLINT(misc-non-private-member-variables-in-classes, cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    /**
    @brief The list of transition table types. One region per transmission table
    is created.
    */
    TransitionTableTypeList transition_tables; //NOLINT(misc-non-private-member-variables-in-classes)

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_auto_start = auto_start; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context = context; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_after_state_transition = has_after_state_transition; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_before_state_transition = has_before_state_transition; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_entry_actions = entry_actions; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_on_event_auto = has_on_event_auto; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_on_event_for = has_on_event_for; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_on_exception = has_on_exception; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_on_exit = has_on_exit; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_on_unprocessed = has_on_unprocessed; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_has_pretty_name = has_pretty_name; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_run_to_completion = run_to_completion; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_align = small_event_max_align; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_size = small_event_max_size; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = transition_tables;

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return machine_conf \
    < \
        std::decay_t<decltype(MAKI_DETAIL_ARG_context)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_entry_actions)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_has_on_event_for)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_transition_tables)> \
    > \
    { \
        MAKI_DETAIL_ARG_auto_start, \
        MAKI_DETAIL_ARG_context, \
        MAKI_DETAIL_ARG_has_after_state_transition, \
        MAKI_DETAIL_ARG_has_before_state_transition, \
        MAKI_DETAIL_ARG_entry_actions, \
        MAKI_DETAIL_ARG_has_on_event_auto, \
        MAKI_DETAIL_ARG_has_on_event_for, \
        MAKI_DETAIL_ARG_has_on_exception, \
        MAKI_DETAIL_ARG_has_on_exit, \
        MAKI_DETAIL_ARG_has_on_unprocessed, \
        MAKI_DETAIL_ARG_has_pretty_name, \
        MAKI_DETAIL_ARG_run_to_completion, \
        MAKI_DETAIL_ARG_small_event_max_align, \
        MAKI_DETAIL_ARG_small_event_max_size, \
        MAKI_DETAIL_ARG_transition_tables \
    };

    template<class EventFilter, detail::event_action_signature Sig, class Action>
    [[nodiscard]] constexpr auto entry_action(const Action& action) const
    {
        const auto new_entry_actions = append
        (
            entry_actions,
            detail::event_action<EventFilter, Action, Sig>{action}
        );

        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_entry_actions new_entry_actions
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_entry_actions
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_v(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::v>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_m(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::m>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_c(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::c>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_ce(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::ce>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_d(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::d>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_de(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::de>(action);
    }

    template<class EventFilter, class Action>
    [[nodiscard]] constexpr auto entry_action_e(const Action& action) const
    {
        return entry_action<EventFilter, detail::event_action_signature::e>(action);
    }

    [[nodiscard]] constexpr auto enable_after_state_transition() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_after_state_transition true
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_after_state_transition
    }

    [[nodiscard]] constexpr auto disable_auto_start() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_auto_start false
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_auto_start
    }

    [[nodiscard]] constexpr auto enable_before_state_transition() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_before_state_transition true
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_before_state_transition
    }

    template<class Context>
    [[nodiscard]] constexpr auto set_context() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_context type_c<Context>
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_context
    }

    [[nodiscard]] constexpr auto disable_run_to_completion() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_run_to_completion false
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_run_to_completion
    }

    [[nodiscard]] constexpr auto enable_pretty_name() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_pretty_name true
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_pretty_name
    }

    [[nodiscard]] constexpr auto enable_on_exception() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_on_exception true
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_on_exception
    }

    [[nodiscard]] constexpr auto enable_on_unprocessed() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_on_unprocessed true
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_on_unprocessed
    }

    [[nodiscard]] constexpr auto set_small_event_max_align(const std::size_t value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_small_event_max_align value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_small_event_max_align
    }

    [[nodiscard]] constexpr auto set_small_event_max_size(const std::size_t value) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_small_event_max_size value
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_small_event_max_size
    }

    template<class... Ts>
    [[nodiscard]] constexpr auto enable_on_event_for() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_on_event_for type_list_c<Ts...>
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_on_event_for
    }

    [[nodiscard]] constexpr auto enable_on_event_auto() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_on_event_auto true
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_on_event_auto
    }

    [[nodiscard]] constexpr auto enable_on_exit() const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_has_on_exit true
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_has_on_exit
    }

    template<class... TransitionTables>
    [[nodiscard]] constexpr auto set_transition_tables(const TransitionTables&... /*tables*/) const
    {
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
#define MAKI_DETAIL_ARG_transition_tables type_list_c<TransitionTables...>
        MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_ARG_transition_tables
    }

#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END
#undef MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN
};

inline constexpr auto default_machine_conf = machine_conf<>{};

namespace detail
{
    template<class T>
    struct is_root_sm_conf
    {
        static constexpr auto value = false;
    };

    template<class... Options>
    struct is_root_sm_conf<machine_conf<Options...>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_root_sm_conf_v = is_root_sm_conf<T>::value;
}

} //namespace

#endif
