// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_FIREWALLED_STATE_HPP
#define MAKI_FIREWALLED_STATE_HPP

#include "detail/context_holder.hpp"
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
    template<class... ContextArgs>
    explicit firewalled_state(ContextArgs&&... ctx_args):
        ctx_holder_(*this, std::forward<ContextArgs>(ctx_args)...),
        impl_(*this, ctx_holder_.get())
    {
    }

    firewalled_state(const firewalled_state&) = delete;
    firewalled_state(firewalled_state&&) = delete;
    ~firewalled_state() = default;
    firewalled_state& operator=(const firewalled_state&) = delete;
    firewalled_state& operator=(firewalled_state&&) = delete;

    auto& context()
    {
        return ctx_holder_.get();
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    static constexpr const auto& conf = ConfHolder::value;

    using conf_detail_type = std::decay_t<decltype(detail::impl_of(conf))>;

    using context_type = typename conf_detail_type::context_type;

    using impl_type = detail::state_impls::composite_no_context<
        ConfHolder,
        detail::iseq<>,
        detail::context_storage::plain>;

    detail::context_holder<
        context_type,
        detail::context_storage::plain,
        detail::impl_of(conf).context_sig>
        ctx_holder_;

    impl_type impl_;
};

} // namespace maki

#endif
