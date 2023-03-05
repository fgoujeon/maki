//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_OBJECT_HOLDER_HPP
#define AWESM_DETAIL_SM_OBJECT_HOLDER_HPP

#include "is_brace_constructible.hpp"
#include <type_traits>

namespace awesm::detail
{

/*
SM objects are states and defs.
All SM objects can be constructed with one of these statements:
    auto obj = object_type{root_sm, context};
    auto obj = object_type{context};
    auto obj = object_type{};
Since we don't want to require SM objects to be copyable or movable, we can't
use a function such as make_sm_object<T>(context). We need this wrapper to
construct the objects.
*/
template<class T>
class sm_object_holder: private T
{
public:
    using object_type = T;

    template
    <
        class RootSm,
        class Context,
        class U = T,
        std::enable_if_t<is_brace_constructible<U, RootSm&, Context&>, bool> = true
    >
    sm_object_holder(RootSm& root_sm, Context& ctx):
        T{root_sm, ctx}
    {
    }

    template
    <
        class RootSm,
        class Context,
        class U = T,
        std::enable_if_t<is_brace_constructible<U, Context&>, bool> = true
    >
    sm_object_holder(RootSm& /*root_sm*/, Context& ctx):
        T{ctx}
    {
    }

    template
    <
        class RootSm,
        class Context,
        class U = T,
        std::enable_if_t<std::is_default_constructible_v<U>, bool> = true
    >
    sm_object_holder(RootSm& /*root_sm*/, Context& /*ctx*/)
    {
    }

    T& get()
    {
        return static_cast<T&>(*this);
    }

    const T& get() const
    {
        return static_cast<const T&>(*this);
    }
};

} //namespace

#endif
