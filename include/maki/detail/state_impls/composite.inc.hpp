//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace maki::detail::state_impls
{

template<class MachineConfHolder, class StateMoldPath, context_storage ParentCtxStorage>
class MAKI_AOS_NAME(composite)
{
public:
    using machine_conf_holder_type = MachineConfHolder;
    using state_mold_path = StateMoldPath;

    static constexpr const auto& mold = machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;
    using mold_type = std::decay_t<decltype(mold)>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using transition_table_type_list = decltype(impl_of(mold).transition_tables);
    using context_type = typename option_set_type::context_type;
    using impl_type = MAKI_AOS_NAME(composite_no_context)<MachineConfHolder, StateMoldPath, ParentCtxStorage>;
    using event_type_set = typename impl_type::event_type_set;
    using deferrable_event_type_set = typename impl_type::deferrable_event_type_set;

    template<class ParentContext>
    MAKI_AOS_NAME(composite)(MAKI_AOS_NAME(machine)<MachineConfHolder>& mach, ParentContext& parent_ctx):
        ctx_holder_(mach, parent_ctx),
        impl_(mach, context())
    {
    }

    MAKI_AOS_NAME(composite)(const MAKI_AOS_NAME(composite)&) = delete;
    MAKI_AOS_NAME(composite)(MAKI_AOS_NAME(composite)&&) = delete;
    MAKI_AOS_NAME(composite)& operator=(const MAKI_AOS_NAME(composite)&) = delete;
    MAKI_AOS_NAME(composite)& operator=(MAKI_AOS_NAME(composite)&&) = delete;
    ~MAKI_AOS_NAME(composite)() = default;

    auto& context()
    {
        return ctx_holder_.get();
    }

    const auto& context() const
    {
        return ctx_holder_.get();
    }

    template<class Event>
    [[nodiscard]] bool defers_event() const
    {
        return impl_.template defers_event<Event>();
    }

    template<class ParentContext>
    void emplace_contexts_with_parent_lifetime(ParentContext& parent_ctx, machine<MachineConfHolder>& mach)
    {
        if constexpr(ctx_lifetime == state_context_lifetime::parent)
        {
            emplace_context(parent_ctx, mach);
        }
    }

    template<class R, class ParentContext, class Event>
    R MAKI_AOS_NAME(enter)
    (
        machine<MachineConfHolder>& mach,
        [[maybe_unused]] ParentContext& parent_ctx,
        const Event& event
    )
    {
        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            emplace_context(parent_ctx, mach);
        }

        MAKI_AOS_CALL impl_.template enter<R>(mach, ctx_holder_.get_deep(), event);
    }

    template<template<class> class AosType, bool Dry, class ParentContext, class Event>
    AosType<bool> MAKI_AOS_NAME(call_internal_action)
    (
        MAKI_AOS_NAME(machine)<MachineConfHolder>& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    )
    {
        MAKI_AOS_RETURN MAKI_AOS_CALL impl_.template MAKI_AOS_NAME(call_internal_action)
        <
            AosType,
            Dry
        >
        (
            mach,
            ctx_holder_.get_deep(),
            event
        );
    }

    template<class R, class ParentContext, class Event>
    R MAKI_AOS_NAME(exit)
    (
        MAKI_AOS_NAME(machine)<MachineConfHolder>& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    )
    {
        MAKI_AOS_CALL impl_.template exit<R>(mach, ctx_holder_.get_deep(), event);

        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            reset_context();
        }
    }

    void reset_contexts_with_parent_lifetime()
    {
        if constexpr(ctx_lifetime == state_context_lifetime::parent)
        {
            reset_context();
        }
    }

    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        return impl_.template region<Index>();
    }

    template<const auto& StateMold>
    [[nodiscard]] const auto& state() const
    {
        return impl_.template state<StateMold>();
    }

    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateMold>();
    }

    [[nodiscard]] bool completed() const
    {
        return impl_.completed();
    }

private:
    template<class ParentContext>
    void emplace_context(ParentContext& parent_ctx, machine<MachineConfHolder>& mach)
    {
        auto& ctx = ctx_holder_.emplace(mach, parent_ctx);

        /*
        Also emplace contexts of substates, which depend on the context we just
        emplaced.
        */
        impl_.emplace_contexts_with_parent_lifetime(ctx, mach);
    }

    void reset_context()
    {
        /*
        First reset contexts of substates, which depend on the context we're
        about to reset.
        */
        impl_.reset_contexts_with_parent_lifetime();

        ctx_holder_.reset();
    }

    static constexpr auto ctx_lifetime = impl_of(mold).context_lifetime;

    static constexpr auto ctx_storage =
        ctx_lifetime == state_context_lifetime::parent ?
        ParentCtxStorage :
        context_storage::optional
    ;

    static constexpr auto ctx_sig = impl_of(mold).context_sig;

    context_holder<context_type, ctx_storage, ctx_sig> ctx_holder_;
    impl_type impl_;
};

} //namespace
