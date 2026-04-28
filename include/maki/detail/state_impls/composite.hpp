//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_COMPOSITE_HPP
#define MAKI_DETAIL_STATE_IMPLS_COMPOSITE_HPP

#include "composite_no_context.hpp"
#include "../context_holder.hpp"
#include "../context_storage.hpp"
#include "../tlu.hpp"
#include "../../state_mold.hpp"
#include "../../context.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<auto Id, const auto& Path, context_storage ParentCtxStorage>
class composite
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& mold = *Id;
    using mold_type = std::decay_t<decltype(mold)>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using transition_table_type_list = decltype(impl_of(mold).transition_tables);
    using context_type = typename option_set_type::context_type;
    using impl_type = composite_no_context<identifier, Path, ParentCtxStorage>;
    using event_type_set = typename impl_type::event_type_set;
    using deferrable_event_type_set = typename impl_type::deferrable_event_type_set;

    template<class Machine, class ParentContext>
    composite(Machine& mach, ParentContext& parent_ctx):
        ctx_holder_(mach, parent_ctx),
        impl_(mach, context())
    {
    }

    composite(const composite&) = delete;
    composite(composite&&) = delete;
    composite& operator=(const composite&) = delete;
    composite& operator=(composite&&) = delete;
    ~composite() = default;

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

    template<class ParentContext, class Machine>
    void emplace_contexts_with_parent_lifetime(ParentContext& parent_ctx, Machine& mach)
    {
        if constexpr(ctx_lifetime == state_context_lifetime::parent)
        {
            ctx_holder_.emplace(mach, parent_ctx);
        }

        impl_.emplace_contexts_with_parent_lifetime(parent_ctx, mach);
    }

    template<class Machine, class ParentContext, class Event>
    void enter
    (
        Machine& mach,
        [[maybe_unused]] ParentContext& parent_ctx,
        const Event& event
    )
    {
        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            auto& ctx = ctx_holder_.emplace(mach, parent_ctx);
            impl_.emplace_contexts_with_parent_lifetime(ctx, mach);
        }

        impl_.enter(mach, ctx_holder_.get_deep(), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    )
    {
        return impl_.template call_internal_action<Dry>(mach, ctx_holder_.get_deep(), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    ) const
    {
        return impl_.template call_internal_action<Dry>(mach, ctx_holder_.get_deep(), event);
    }

    template<class Machine, class ParentContext, class Event>
    void exit
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    )
    {
        impl_.exit(mach, ctx_holder_.get_deep(), event);

        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            impl_.reset_contexts_with_parent_lifetime();
            ctx_holder_.reset();
        }
    }

    void reset_contexts_with_parent_lifetime()
    {
        impl_.reset_contexts_with_parent_lifetime();

        if constexpr(ctx_lifetime == state_context_lifetime::parent)
        {
            ctx_holder_.reset();
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

#endif
