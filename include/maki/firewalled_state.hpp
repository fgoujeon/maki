// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_FIREWALLED_STATE_HPP
#define MAKI_FIREWALLED_STATE_HPP

#include "detail/context_storage.hpp"
#include "detail/friendly_impl.hpp"
#include "detail/iseq.hpp"
#include "detail/state_impls/composite_no_context.hpp"
#include <type_traits>

namespace maki
{

template<class ConfHolder>
class firewalled_state
{
public:
    template<class Machine, class ParentContext>
    explicit firewalled_state(Machine& mach, ParentContext& parent_ctx):
        impl_(mach, parent_ctx)
    {
    }

    firewalled_state(const firewalled_state&) = delete;
    firewalled_state(firewalled_state&&) = delete;
    ~firewalled_state() = default;
    firewalled_state& operator=(const firewalled_state&) = delete;
    firewalled_state& operator=(firewalled_state&&) = delete;

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    using impl_type = detail::state_impls::composite_no_context<
        ConfHolder,
        detail::iseq<>,
        detail::context_storage::plain>;

    impl_type impl_;
};

} // namespace maki

#endif
