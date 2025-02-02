//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_HPP
#define MAKI_DETAIL_SIMPLE_STATE_HPP

#include "simple_state_no_context.hpp"
#include "context_holder.hpp"
#include <type_traits>

namespace maki::detail
{

template<auto Id>
class simple_state
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& conf = *Id;
    using option_set_type = std::decay_t<decltype(impl_of(conf))>;
    using context_type = typename option_set_type::context_type;

    static constexpr auto context_sig = impl_of(conf).context_sig;

    template<class... Args>
    simple_state(Args&... args):
        ctx_holder_(args...)
    {
    }

    simple_state(const simple_state&) = delete;
    simple_state(simple_state&&) = delete;
    simple_state& operator=(const simple_state&) = delete;
    simple_state& operator=(simple_state&&) = delete;
    ~simple_state() = default;

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

    template<class Event>
    static constexpr bool has_internal_action_for_event()
    {
        return impl_type::template has_internal_action_for_event<Event>();
    }

private:
    using impl_type = simple_state_no_context<identifier>;

    context_holder<context_type, context_sig> ctx_holder_;
    impl_type impl_;
};

} //namespace

#endif
