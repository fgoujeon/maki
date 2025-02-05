//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_COMPOSITE_STATE_HPP
#define MAKI_DETAIL_COMPOSITE_STATE_HPP

#include "tlu.hpp"
#include "composite_state_no_context.hpp"
#include "context_holder.hpp"
#include "../state_conf.hpp"
#include <type_traits>

namespace maki::detail
{

template<auto Id, const auto& Path>
class composite_state
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& conf = *Id;
    using conf_type = std::decay_t<decltype(conf)>;
    using option_set_type = std::decay_t<decltype(impl_of(conf))>;
    using transition_table_type_list = decltype(impl_of(conf).transition_tables);
    using context_type = typename option_set_type::context_type;

    template<class Machine, class ParentContext>
    composite_state(Machine& mach, ParentContext& parent_ctx):
        ctx_holder_(mach, parent_ctx),
        impl_(mach, context())
    {
    }

    composite_state(const composite_state&) = delete;
    composite_state(composite_state&&) = delete;
    composite_state& operator=(const composite_state&) = delete;
    composite_state& operator=(composite_state&&) = delete;
    ~composite_state() = default;

    auto& context()
    {
        return ctx_holder_.get();
    }

    const auto& context() const
    {
        return ctx_holder_.get();
    }

    template<const auto& RegionPath>
    [[nodiscard]] bool running() const
    {
        return impl_.template running<RegionPath>();
    }

    template<class Machine, class ParentContext, class Event>
    void call_entry_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_.call_entry_action(mach, context(), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    void call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    )
    {
        impl_.template call_internal_action<Dry>(mach, context(), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    void call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event,
        bool& processed
    )
    {
        impl_.template call_internal_action<Dry>(mach, context(), event, processed);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    void call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    ) const
    {
        impl_.template call_internal_action<Dry>(mach, context(), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    void call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event,
        bool& processed
    ) const
    {
        impl_.template call_internal_action<Dry>(mach, context(), event, processed);
    }

    template<class Machine, class ParentContext, class Event>
    void call_exit_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    )
    {
        impl_.call_exit_action(mach, context(), event);
    }

    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        return impl_.template region<Index>();
    }

    template<const auto& StateConf>
    [[nodiscard]] const auto& state() const
    {
        return impl_.template state<StateConf>();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateConf>();
    }

    [[nodiscard]] bool running() const
    {
        return impl_.running();
    }

    template<class /*Event*/>
    static constexpr bool has_internal_action_for_event()
    {
        return true;
    }

private:
    using impl_type = composite_state_no_context<identifier, Path>;

    context_holder<context_type, impl_of(conf).context_sig> ctx_holder_;
    impl_type impl_;
};

} //namespace

#endif
