//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONTEXT_HOLDER_HPP
#define MAKI_DETAIL_CONTEXT_HOLDER_HPP

#include "type_traits.hpp"
#include <type_traits>

namespace maki::detail
{

struct context_holder_machine_tag_t{};
inline constexpr auto context_holder_machine_tag = context_holder_machine_tag_t{};


/*
Contexts can be constructed with one of these statements:
    auto obj = context{mach, args...};
    auto obj = context{args...};
*/
template<class T>
class context_holder
{
public:
    using context_type = T;

    template
    <
        class Machine,
        class... Args,
        std::enable_if_t<is_brace_constructible<T, Machine&, Args...>, bool> = true
    >
    context_holder(context_holder_machine_tag_t /*tag*/, Machine& mach, Args&&... args):
        ctx_{mach, std::forward<Args>(args)...}
    {
    }

    template
    <
        class Machine,
        class... Args,
        std::enable_if_t<is_brace_constructible<T, Args...>, bool> = true
    >
    context_holder(context_holder_machine_tag_t /*tag*/, Machine& /*mach*/, Args&&... args):
        ctx_{std::forward<Args>(args)...}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        std::enable_if_t<is_brace_constructible<T, Machine&, ParentContext&>, bool> = true
    >
    context_holder(Machine& mach, ParentContext& parent_ctx):
        ctx_{mach, parent_ctx}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        std::enable_if_t<is_brace_constructible<T, Machine&>, bool> = true
    >
    context_holder(Machine& mach, ParentContext& /*parent_ctx*/):
        ctx_{mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        std::enable_if_t<is_brace_constructible<T, ParentContext&>, bool> = true
    >
    context_holder(Machine& /*mach*/, ParentContext& parent_ctx):
        ctx_{parent_ctx}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        class U = T,
        std::enable_if_t<std::is_default_constructible_v<U>, bool> = true
    >
    context_holder(Machine& /*mach*/, ParentContext& /*ctx*/)
    {
    }

    template
    <
        class Machine,
        class U = T,
        std::enable_if_t<std::is_reference_v<U>, bool> = true
    >
    context_holder(Machine& /*mach*/, T& ctx):
        ctx_{ctx}
    {
    }

    T& get()
    {
        return ctx_;
    }

    const T& get() const
    {
        return ctx_;
    }

private:
    T ctx_;
};

template<class T>
class machine_context_holder
{
public:
    using context_type = T;

    T& get()
    {
        return ctx_;
    }

    const T& get() const
    {
        return ctx_;
    }

private:
    T ctx_;
};

} //namespace

#endif
