//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP
#define MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP

#include "simple_no_context.hpp"
#include "../context_holder.hpp"
#include "../context_storage.hpp"
#include "../../context.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<auto Id, context_storage ParentCtxStorage>
class simple
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& mold = *Id;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using context_type = typename option_set_type::context_type;

    using impl_type = simple_no_context<identifier>;

    using event_type_set = typename impl_type::event_type_set;

    static constexpr auto context_sig = impl_of(mold).context_sig;

    template<class... Args>
    simple(Args&... args):
        ctx_holder_(args...)
    {
    }

    simple(const simple&) = delete;
    simple(simple&&) = delete;
    simple& operator=(const simple&) = delete;
    simple& operator=(simple&&) = delete;
    ~simple() = default;

    auto& context()
    {
        return ctx_holder_.get();
    }

    const auto& context() const
    {
        return ctx_holder_.get();
    }

    template<class ParentContext, class Machine>
    void emplace_contexts_with_parent_lifetime(ParentContext& parent_ctx, Machine& mach)
    {
        if constexpr(ctx_lifetime == state_context_lifetime::parent)
        {
            ctx_holder_.emplace(mach, parent_ctx);
        }
    }

    template<class Machine, class ParentContext, class Event>
    void enter(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            ctx_holder_.emplace(mach, parent_ctx);
        }

        impl_.enter(mach, ctx_holder_.get_deep(), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        return impl_.template call_internal_action<Dry>(mach, ctx_holder_.get_deep(), event);
    }

    template<class Machine, class ParentContext, class Event>
    void exit(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_.exit(mach, ctx_holder_.get_deep(), event);

        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            ctx_holder_.reset();
        }
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
    impl_type impl_;
};

} //namespace

#endif
