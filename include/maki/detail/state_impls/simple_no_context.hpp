//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_SIMPLE_NO_CONTEXT_HPP
#define MAKI_DETAIL_STATE_IMPLS_SIMPLE_NO_CONTEXT_HPP

#include "../event_action.hpp"
#include "../tuple.hpp"
#include "../maybe_bool_util.hpp"
#include "../tlu.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<auto Id>
class simple_no_context
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& mold = *Id;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;

    template<class... Args>
    constexpr simple_no_context(Args&... /*args*/)
    {
    }

    simple_no_context(const simple_no_context&) = default;
    simple_no_context(simple_no_context&&) = default;
    simple_no_context& operator=(const simple_no_context&) = default;
    simple_no_context& operator=(simple_no_context&&) = default;
    ~simple_no_context() = default;

    template<class Machine, class Context, class Event>
    static void call_entry_action(Machine& mach, Context& ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<entry_action_ptr_constant_list>)
        {
            /*
            If at least one entry action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<entry_action_ptr_constant_list>
            (
                mach,
                ctx,
                event
            );
        }
    }

    template<bool Dry, class Machine, class Context, class Event, class... MaybeBool>
    static void call_internal_action(Machine& mach, Context& ctx, const Event& event, MaybeBool&... processed)
    {
        /*
        Caller is supposed to check an interal action exists for the given event
        type before calling this function.
        */
        static_assert(!tlu::empty_v<internal_action_ptr_constant_list>);

        if constexpr(!Dry)
        {
            call_matching_event_action<internal_action_ptr_constant_list>
            (
                mach,
                ctx,
                event
            );
        }

        maybe_bool_util::set_to_true(processed...);
    }

    template<class Machine, class Context, class Event>
    static void call_exit_action(Machine& mach, Context& ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<exit_action_ptr_constant_list>)
        {
            /*
            If at least one exit action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<exit_action_ptr_constant_list>
            (
                mach,
                ctx,
                event
            );
        }
    }

    static constexpr bool completed()
    {
        // Simple states are always completed.
        return true;
    }

    template<class Event>
    static constexpr bool has_internal_action_for_event()
    {
        return tlu::contains_if_v
        <
            internal_action_ptr_constant_list,
            event_action_traits::for_event<Event>::template has_containing_event_set
        >;
    }

private:
    static constexpr auto entry_actions = impl_of(mold).entry_actions;
    using entry_action_ptr_constant_list = tuple_to_element_ptr_constant_list_t<entry_actions>;

    static constexpr auto internal_actions = impl_of(mold).internal_actions;
    using internal_action_ptr_constant_list = tuple_to_element_ptr_constant_list_t<internal_actions>;

    static constexpr auto exit_actions = impl_of(mold).exit_actions;
    using exit_action_ptr_constant_list = tuple_to_element_ptr_constant_list_t<exit_actions>;
};

} //namespace

#endif
