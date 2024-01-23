//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_HPP
#define MAKI_DETAIL_SIMPLE_STATE_HPP

#include "call_member.hpp"
#include "maybe_bool_util.hpp"
#include "tlu.hpp"
#include "context_holder.hpp"
#include "../type_patterns.hpp"
#include "../null.hpp"

namespace maki::detail
{

/*
Implementation of a non-composite state
*/
template<const auto& Conf, class Parent>
class simple_state
{
public:
    using option_set_type = std::decay_t<decltype(opts(Conf))>;
    using parent_context_type = typename Parent::context_type;
    using context_type = state_traits::context_t<Conf, parent_context_type>;

    template<class Machine, class... ContextArgs>
    simple_state(Machine& mach, ContextArgs&&... ctx_args):
        ctx_holder_(context_holder_machine_tag, mach, std::forward<ContextArgs>(ctx_args)...)
    {
    }

    template<class Machine>
    simple_state(Machine& mach, parent_context_type& parent_ctx):
        ctx_holder_(mach, parent_ctx)
    {
    }

    auto& context()
    {
        return ctx_holder_.get();
    }

    const auto& context() const
    {
        return ctx_holder_.get();
    }

    template<class Machine, class Context, class Event>
    void call_entry_action(Machine& mach, Context& /*ctx*/, const Event& event)
    {
        call_state_action
        (
            opts(Conf).entry_actions,
            mach,
            context(),
            event
        );
    }

    template<class Machine, class Context, class Event, class... MaybeBool>
    void call_internal_action(Machine& mach, Context& /*ctx*/, const Event& event, MaybeBool&... processed)
    {
        call_state_action
        (
            opts(Conf).internal_actions,
            mach,
            context(),
            event
        );
        maybe_bool_util::set_to_true(processed...);
    }

    template<class Machine, class Context, class Event>
    void call_exit_action(Machine& mach, Context& /*ctx*/, const Event& event)
    {
        call_state_action
        (
            opts(Conf).exit_actions,
            mach,
            context(),
            event
        );
    }

    template<class Event>
    static constexpr bool has_internal_action_for_event()
    {
        return tuple_contains_if
        (
            opts(Conf).internal_actions,
            [](const auto& act)
            {
                return matches_pattern(type_c<Event>, act.event_filter);
            }
        );
    }

    static constexpr const auto& conf = Conf;

private:
    context_holder<context_type> ctx_holder_;
};

} //namespace

#endif
