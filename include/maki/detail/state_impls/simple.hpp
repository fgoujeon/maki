//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP
#define MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP

#include "simple_no_context.hpp"
#include "../context_holder.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<auto Id>
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

    template<class Machine, class ParentContext, class Event>
    void call_entry_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_.call_entry_action(mach, context(), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event, class... MaybeBool>
    void call_internal_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event, MaybeBool&... processed)
    {
        impl_.template call_internal_action<Dry>(mach, context(), event, processed...);
    }

    template<class Machine, class ParentContext, class Event>
    void call_exit_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_.call_exit_action(mach, context(), event);
    }

    static constexpr bool completed()
    {
        // Simple states are always completed.
        return true;
    }

    static constexpr const auto& event_types()
    {
        return impl_type::event_types();
    }

private:
    context_holder<context_type, context_sig> ctx_holder_;
    impl_type impl_;
};

} //namespace

#endif
