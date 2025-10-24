//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_COMPOSITE_HPP
#define MAKI_DETAIL_STATE_IMPLS_COMPOSITE_HPP

#include "composite_no_context.hpp"
#include "../context_holder.hpp"
#include "../tlu.hpp"
#include "../../state_mold.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<auto Id, const auto& Path>
class composite
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& mold = *Id;
    using mold_type = std::decay_t<decltype(mold)>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using transition_table_type_list = decltype(impl_of(mold).transition_tables);
    using context_type = typename option_set_type::context_type;
    using impl_type = composite_no_context<identifier, Path>;
    using event_type_set = typename impl_type::event_type_set;

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

    template<class Machine, class ParentContext, class Event>
    void call_entry_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_.call_entry_action(mach, context(), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    )
    {
        return impl_.template call_internal_action<Dry>(mach, context(), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action
    (
        Machine& mach,
        ParentContext& /*parent_ctx*/,
        const Event& event
    ) const
    {
        return impl_.template call_internal_action<Dry>(mach, context(), event);
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
    context_holder<context_type, impl_of(mold).context_sig> ctx_holder_;
    impl_type impl_;
};

} //namespace

#endif
