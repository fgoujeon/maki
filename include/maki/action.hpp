//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::action class template
*/

#ifndef MAKI_ACTION_HPP
#define MAKI_ACTION_HPP

#include "detail/signature_macros.hpp"
#include "detail/call.hpp"
#include "null.hpp"

namespace maki
{

/**
@brief The set of arguments taken by an action callable.
*/
enum class action_signature: char
{
    ///`void action()`
    v,

    ///`void action(context&)`
    c,

    ///`void action(context&, machine&)`
    cm,

    ///`void action(context&, machine&, const event&)`
    cme,

    ///`void action(context&, const event&)`
    ce,

    ///`void action(machine&)`
    m,

    ///`void action(machine&, const event&)`
    me,

    ///`void action(const event&)`
    e,
};

/**
@brief Represents an action to be given to `maki::transition_table`.
Use the builder functions (`maki::action_v()` and its variants) instead of using
this type directly.
*/
template<action_signature Sig, class Callable>
struct action
{
    static constexpr action_signature signature = Sig;
    Callable callable;
};

#define MAKI_DETAIL_X(name) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    /** \
    @brief Makes a `maki::action` with the indicated signature and given  \
    callable. \
    */ \
    template<class Callable> \
    constexpr action<action_signature::name, Callable> action_##name(const Callable& callable) \
    { \
        return {callable}; \
    }
MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

namespace detail
{
    inline constexpr auto null_action = action_v([]{});

    template<action_signature Sig, class Callable>
    constexpr const auto& to_action(const action<Sig, Callable>& act)
    {
        return act;
    }

    constexpr const auto& to_action(null_t /*ignored*/)
    {
        return null_action;
    }

    template<class T>
    struct is_action
    {
        static constexpr auto value = false;
    };

    template<action_signature Sig, class Callable>
    struct is_action<action<Sig, Callable>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr bool is_action_v = is_action<T>::value;

    template
    <
        class Action,
        class Context,
        class Machine,
        class Event
    >
    void call_action
    (
        const Action& act,
        Context& ctx,
        Machine& mach,
        const Event& event
    )
    {
        call_callable<action_signature, Action::signature>
        (
            act.callable,
            ctx,
            mach,
            event
        );
    }
}

} //namespace

#endif
