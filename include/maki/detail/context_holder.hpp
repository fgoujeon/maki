//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONTEXT_HOLDER_HPP
#define MAKI_DETAIL_CONTEXT_HOLDER_HPP

#include "../context.hpp"
#include <type_traits>
#include <utility>

namespace maki::detail
{

template<class T, auto ContextSignature>
class context_holder
{
public:
    template
    <
        class Machine,
        class... Args,
        auto ContextSig = ContextSignature,
        std::enable_if_t<ContextSig == machine_context_signature::a, bool> = true
    >
    context_holder(Machine& /*mach*/, Args&&... args):
        ctx_{std::forward<Args>(args)...}
    {
    }

    template
    <
        class Machine,
        class... Args,
        auto ContextSig = ContextSignature,
        std::enable_if_t<ContextSig == machine_context_signature::am, bool> = true
    >
    context_holder(Machine& mach, Args&&... args):
        ctx_{std::forward<Args>(args)..., mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto ContextSig = ContextSignature,
        std::enable_if_t<ContextSig == state_context_signature::c, bool> = true
    >
    context_holder(Machine& /*mach*/, ParentContext& parent_ctx):
        ctx_{parent_ctx}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto ContextSig = ContextSignature,
        std::enable_if_t<ContextSig == state_context_signature::cm, bool> = true
    >
    context_holder(Machine& mach, ParentContext& parent_ctx):
        ctx_{parent_ctx, mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto ContextSig = ContextSignature,
        std::enable_if_t<ContextSig == state_context_signature::m, bool> = true
    >
    context_holder(Machine& mach, ParentContext& /*parent_ctx*/):
        ctx_{mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto ContextSig = ContextSignature,
        std::enable_if_t<ContextSig == state_context_signature::v, bool> = true
    >
    context_holder(Machine& /*mach*/, ParentContext& /*parent_ctx*/)
    {
    }

    auto& get()
    {
        return ctx_;
    }

    const auto& get() const
    {
        return ctx_;
    }

private:
    T ctx_;
};

} //namespace

#endif
