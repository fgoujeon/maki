// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP
#define MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP

#include "../../context.hpp"
#include "../context_storage.hpp"
#include "../context_tree.hpp"
#include "../machine_fwd.hpp"
#include "simple_no_context.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<class MachineConfHolder, class StateMoldPath>
class simple
{
public:
    using machine_conf_holder_type = MachineConfHolder;
    using state_mold_path = StateMoldPath;

    static constexpr const auto& mold =
        machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using context_type = typename option_set_type::context_type;

    using impl_type = simple_no_context<MachineConfHolder, StateMoldPath>;

    using event_type_set = typename impl_type::event_type_set;

    using deferrable_event_type_set =
        typename impl_type::deferrable_event_type_set;

    template<class... Args>
    simple(context_tree<MachineConfHolder>& ctx_tree): ctx_tree_(ctx_tree)
    {
    }

    simple(const simple&) = delete;
    simple(simple&&) = delete;
    simple& operator=(const simple&) = delete;
    simple& operator=(simple&&) = delete;
    ~simple() = default;

    auto& context()
    {
        return ctx_tree_.template context_at<StateMoldPath>();
    }

    const auto& context() const
    {
        return ctx_tree_.template context_at<StateMoldPath>();
    }

    template<class Event>
    [[nodiscard]] static constexpr bool defers_event()
    {
        return impl_type::template defers_event<Event>();
    }

    template<class ParentContext, class Machine>
    void emplace_contexts_with_parent_lifetime(
        ParentContext& parent_ctx,
        Machine& mach)
    {
        if constexpr (ctx_lifetime == state_context_lifetime::parent)
        {
            ctx_tree_.template context_holder_at<StateMoldPath>().emplace(
                mach,
                parent_ctx);
        }
    }

    template<class Machine, class ParentContext, class Event>
    void enter(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        if constexpr (ctx_lifetime == state_context_lifetime::state_activity)
        {
            ctx_tree_.template context_holder_at<StateMoldPath>().emplace(
                mach,
                parent_ctx);
        }

        impl_type::enter(
            mach,
            ctx_tree_.template context_holder_at<StateMoldPath>().get_deep(),
            event);
    }

    template<class Machine, class ParentContext, class Event>
    bool process_event(
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event)
    {
        return impl_type::process_event(
            mach,
            ctx_tree_.template context_holder_at<StateMoldPath>().get_deep(),
            event);
    }

    template<class Machine, class ParentContext, class Event>
    bool check_event(
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event) const
    {
        return impl_type::check_event(
            mach,
            ctx_tree_.template context_holder_at<StateMoldPath>().get_deep(),
            event);
    }

    template<class Machine, class ParentContext, class Event>
    void exit(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_type::exit(
            mach,
            ctx_tree_.template context_holder_at<StateMoldPath>().get_deep(),
            event);

        if constexpr (ctx_lifetime == state_context_lifetime::state_activity)
        {
            ctx_tree_.template context_holder_at<StateMoldPath>().reset();
        }
    }

    void reset_contexts_with_parent_lifetime()
    {
        if constexpr (ctx_lifetime == state_context_lifetime::parent)
        {
            ctx_tree_.template context_holder_at<StateMoldPath>().reset();
        }
    }

    static constexpr bool completed()
    {
        // Simple states are always completed.
        return true;
    }

private:
    static constexpr auto ctx_lifetime = impl_of(mold).context_lifetime;

    context_tree<MachineConfHolder>& ctx_tree_;
};

} // namespace maki::detail::state_impls

#endif
