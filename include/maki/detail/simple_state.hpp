//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_HPP
#define MAKI_DETAIL_SIMPLE_STATE_HPP

#include "call_member.hpp"
#include "maybe_bool_util.hpp"
#include "tlu.hpp"
#include "root_tag.hpp"
#include "../type_patterns.hpp"
#include "../null.hpp"

namespace maki::detail
{

/*
Implementation of a non-composite state
*/
template<const auto& Conf>
class simple_state
{
public:
    using option_set_type = std::decay_t<decltype(opts(Conf))>;
    using context_type = state_traits::context_t<Conf>;

    template
    <
        class Machine,
        class... Args,
        std::enable_if_t<is_brace_constructible<context_type, Machine&, Args...>, bool> = true
    >
    simple_state(root_tag_t /*tag*/, Machine& mach, Args&&... args):
        ctx_{mach, std::forward<Args>(args)...}
    {
    }

    template
    <
        class Machine,
        class... Args,
        std::enable_if_t<is_brace_constructible<context_type, Args...>, bool> = true
    >
    simple_state(root_tag_t /*tag*/, Machine& /*mach*/, Args&&... args):
        ctx_{std::forward<Args>(args)...}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        std::enable_if_t<is_brace_constructible<context_type, Machine&, ParentContext&>, bool> = true
    >
    simple_state(non_root_tag_t /*tag*/, Machine& mach, ParentContext& parent_ctx):
        ctx_{mach, parent_ctx}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        class U = context_type,
        std::enable_if_t<!std::is_same_v<U, Machine> && is_brace_constructible<context_type, Machine&>, bool> = true
    >
    simple_state(non_root_tag_t /*tag*/, Machine& mach, ParentContext& /*parent_ctx*/):
        ctx_{mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        class U = context_type,
        std::enable_if_t<!std::is_same_v<U, ParentContext> && is_brace_constructible<U, ParentContext&>, bool> = true
    >
    simple_state(non_root_tag_t /*tag*/, Machine& /*mach*/, ParentContext& parent_ctx):
        ctx_{parent_ctx}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        class U = context_type,
        std::enable_if_t<std::is_default_constructible_v<U>, bool> = true
    >
    simple_state(non_root_tag_t /*tag*/, Machine& /*mach*/, ParentContext& /*ctx*/)
    {
    }

    template<class ParentContext>
    auto& context(ParentContext& parent_ctx)
    {
        if constexpr(std::is_void_v<typename option_set_type::context_type>)
        {
            return parent_ctx;
        }
        else
        {
            return ctx_;
        }
    }

    template<class ParentContext>
    const auto& context(ParentContext& parent_ctx) const
    {
        if constexpr(std::is_void_v<typename option_set_type::context_type>)
        {
            return parent_ctx;
        }
        else
        {
            return ctx_;
        }
    }

    template<class Machine, class Context, class Event>
    void call_entry_action(Machine& mach, Context& parent_ctx, const Event& event)
    {
        call_state_action
        (
            opts(Conf).entry_actions,
            mach,
            context(parent_ctx),
            event
        );
    }

    template<class Machine, class Context, class Event, class... MaybeBool>
    void call_internal_action(Machine& mach, Context& parent_ctx, const Event& event, MaybeBool&... processed)
    {
        call_state_action
        (
            opts(Conf).internal_actions,
            mach,
            context(parent_ctx),
            event
        );
        maybe_bool_util::set_to_true(processed...);
    }

    template<class Machine, class Context, class Event>
    void call_exit_action(Machine& mach, Context& parent_ctx, const Event& event)
    {
        call_state_action
        (
            opts(Conf).exit_actions,
            mach,
            context(parent_ctx),
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
                return matches_pattern(type<Event>, act.event_filter);
            }
        );
    }

    static constexpr const auto& conf = Conf;

private:
    context_type ctx_;
};

} //namespace

#endif
