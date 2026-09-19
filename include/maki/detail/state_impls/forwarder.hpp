// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_FORWARDER_HPP
#define MAKI_DETAIL_STATE_IMPLS_FORWARDER_HPP

#include "../../state_mold.hpp"
#include "../context_storage.hpp"
#include "../machine_conf_tree.hpp"
#include "../machine_fwd.hpp"
#include "../tlu.hpp"
#include "../friendly_impl.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentCtxStorage>
class forwarder
{
public:
    using machine_conf_holder_type = MachineConfHolder;
    using state_mold_path = StateMoldPath;

    static constexpr const auto& mold =
        machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;
    using mold_type = std::decay_t<decltype(mold)>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using forwarder_type = typename option_set_type::forwarder_type;
    using context_type = typename option_set_type::context_type;
    using event_type_set = impl_of_t<decltype(forwarder_type::event_type_set)>;
    using deferrable_event_type_set =
        impl_of_t<decltype(forwarder_type::deferrable_event_type_set)>;

    template<class ParentContext>
    forwarder(machine<MachineConfHolder>& mach, ParentContext& parent_ctx):
        impl_(mach, parent_ctx)
    {
    }

    forwarder(const forwarder&) = delete;
    forwarder(forwarder&&) = delete;
    forwarder& operator=(const forwarder&) = delete;
    forwarder& operator=(forwarder&&) = delete;
    ~forwarder() = default;

    template<class Event>
    [[nodiscard]] bool defers_event() const
    {
        return impl_.template defers_event<Event>();
    }

    template<class ParentContext>
    void emplace_contexts_with_parent_lifetime(
        ParentContext& parent_ctx,
        machine<MachineConfHolder>& mach)
    {
        impl_.emplace_contexts_with_parent_lifetime(parent_ctx, mach);
    }

    template<class ParentContext, class Event>
    void enter(
        machine<MachineConfHolder>& mach,
        ParentContext& parent_ctx,
        const Event& event)
    {
        impl_.enter(mach, parent_ctx, event);
    }

    template<class ParentContext, class Event>
    bool process_event(
        machine<MachineConfHolder>& mach,
        ParentContext& parent_ctx,
        const Event& event)
    {
        return impl_.process_event(mach, parent_ctx, event);
    }

    template<class ParentContext, class Event>
    bool check_event(
        machine<MachineConfHolder>& mach,
        ParentContext& parent_ctx,
        const Event& event) const
    {
        return impl_.check_event(mach, parent_ctx, event);
    }

    template<class ParentContext, class Event>
    void exit(
        machine<MachineConfHolder>& mach,
        ParentContext& parent_ctx,
        const Event& event)
    {
        impl_.exit(mach, parent_ctx, event);
    }

    void reset_contexts_with_parent_lifetime()
    {
        impl_.reset_contexts_with_parent_lifetime();
    }

//    template<int Index>
//    [[nodiscard]] const auto& region() const
//    {
//        return impl_.template region<Index>();
//    }
//
//    template<const auto& StateMold>
//    [[nodiscard]] const auto& state() const
//    {
//        return impl_.template state<StateMold>();
//    }
//
//    template<const auto& StateMold>
//    [[nodiscard]] bool is() const
//    {
//        return impl_.template is<StateMold>();
//    }
//
//    [[nodiscard]] bool completed() const
//    {
//        return impl_.completed();
//    }

private:
    forwarder_type impl_;
};

} // namespace maki::detail::state_impls

#endif
