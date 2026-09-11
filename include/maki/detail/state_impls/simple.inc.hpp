//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace maki::detail::state_impls
{

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
class MAKI_AOS_NAME(simple)
{
public:
    using machine_conf_holder_type = MachineConfHolder;
    using state_mold_path = StateMoldPath;

    static constexpr const auto& mold = machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using context_type = typename option_set_type::context_type;

    using impl_type = simple_no_context<MachineConfHolder, StateMoldPath>;

    using event_type_set = typename impl_type::event_type_set;

    using deferrable_event_type_set =
        typename impl_type::deferrable_event_type_set
    ;

    static constexpr auto context_sig = impl_of(mold).context_sig;

    template<class... Args>
    MAKI_AOS_NAME(simple)(Args&... args):
        ctx_holder_(args...)
    {
    }

    MAKI_AOS_NAME(simple)(const MAKI_AOS_NAME(simple)&) = delete;
    MAKI_AOS_NAME(simple)(MAKI_AOS_NAME(simple)&&) = delete;
    MAKI_AOS_NAME(simple)& operator=(const MAKI_AOS_NAME(simple)&) = delete;
    MAKI_AOS_NAME(simple)& operator=(MAKI_AOS_NAME(simple)&&) = delete;
    ~MAKI_AOS_NAME(simple)() = default;

    auto& context()
    {
        return ctx_holder_.get();
    }

    const auto& context() const
    {
        return ctx_holder_.get();
    }

    template<class Event>
    [[nodiscard]] static constexpr bool defers_event()
    {
        return impl_type::template defers_event<Event>();
    }

    template<class ParentContext, class Machine>
    void emplace_contexts_with_parent_lifetime(ParentContext& parent_ctx, Machine& mach)
    {
        if constexpr(ctx_lifetime == state_context_lifetime::parent)
        {
            ctx_holder_.emplace(mach, parent_ctx);
        }
    }

    template<class R, class Machine, class ParentContext, class Event>
    R enter(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            ctx_holder_.emplace(mach, parent_ctx);
        }

        MAKI_AOS_CALL impl_type::template MAKI_AOS_NAME(enter)<R>(mach, ctx_holder_.get_deep(), event);
    }

    template<class R, class Machine, class Context, class Event>
    R async_enter(Machine& mach, Context& ctx, const Event& event)
    {
        MAKI_AOS_CALL enter<R>(mach, ctx, event);
    }

    template<template<class> class AosType, bool Dry, class Machine, class ParentContext, class Event>
    AosType<bool> call_internal_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        return impl_type::template call_internal_action<AosType, Dry>(mach, ctx_holder_.get_deep(), event);
    }

    template<template<class> class AsyncType, bool Dry, class Machine, class ParentContext, class Event>
    AsyncType<bool> async_call_internal_action(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        MAKI_AOS_RETURN MAKI_AOS_CALL call_internal_action<AsyncType, Dry>(mach, parent_ctx, event);
    }

    template<class R, class Machine, class ParentContext, class Event>
    R exit(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        MAKI_AOS_CALL impl_type::template MAKI_AOS_NAME(exit)<R>(mach, ctx_holder_.get_deep(), event);

        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            ctx_holder_.reset();
        }
    }

    template<class R, class Machine, class Context, class Event>
    R async_exit(Machine& mach, Context& ctx, const Event& event)
    {
        MAKI_AOS_CALL exit<R>(mach, ctx, event);
    }

    void reset_contexts_with_parent_lifetime()
    {
        if constexpr(ctx_lifetime == state_context_lifetime::parent)
        {
            ctx_holder_.reset();
        }
    }

    static constexpr bool completed()
    {
        // Simple states are always completed.
        return true;
    }

private:
    static constexpr auto ctx_lifetime = impl_of(mold).context_lifetime;

    static constexpr auto ctx_storage =
        ctx_lifetime == state_context_lifetime::parent ?
        ParentCtxStorage :
        context_storage::optional
    ;

    context_holder<context_type, ctx_storage, context_sig> ctx_holder_;
};

} //namespace
