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
#include "../../state_builder.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<auto Id, const auto& Path>
class composite
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& builder = *Id;
    using builder_type = std::decay_t<decltype(builder)>;
    using option_set_type = std::decay_t<decltype(impl_of(builder))>;
    using transition_table_type_list = decltype(impl_of(builder).transition_tables);
    using context_type = typename option_set_type::context_type;

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

    template<const auto& StateBuilder>
    [[nodiscard]] const auto& state() const
    {
        return impl_.template state<StateBuilder>();
    }

    template<const auto& StateBuilder>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateBuilder>();
    }

    [[nodiscard]] bool completed() const
    {
        return impl_.completed();
    }

    template<class /*Event*/>
    static constexpr bool has_internal_action_for_event()
    {
        return true;
    }

private:
    using impl_type = composite_no_context<identifier, Path>;

    context_holder<context_type, impl_of(builder).context_sig> ctx_holder_;
    impl_type impl_;
};

} //namespace

#endif
