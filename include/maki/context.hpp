//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_CONTEXT_HPP
#define MAKI_CONTEXT_HPP

namespace maki
{

/**
@brief The set of arguments taken by the constructor of a machine context.
*/
enum class machine_context_signature: char
{
    ///`context(MachineConstructorArgs&&...)`
    a,

    ///`context(MachineConstructorArgs&&..., machine&)`
    am
};

/**
@brief The set of arguments taken by the constructor of a state context.
*/
enum class state_context_signature: char
{
    ///`context()`
    v,

    ///`context(context&)`
    c,

    ///`context(context&, machine&)`
    cm,

    ///`context(machine&)`
    m
};

/**
@brief The lifetime of a context instance.

@note Available from Maki 1.1.0.
*/
enum class state_context_lifetime: char
{
    /**
    The lifetime of the context is tied to its parent. This is the default.

    Note that the lifetime of the root context (i.e. the context of the
    `machine`) is tied to the `machine`.
    */
    parent,

    /**
    The lifetime of the context is tied to the state activity, that is, the
    context is instantiated right before the state is entered and uninstantiated
    right after the state is exited.
    */
    state_activity
};

} //namespace

#endif
