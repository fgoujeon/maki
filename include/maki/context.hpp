//Copyright Florian Goujeon 2021 - 2025.
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

} //namespace

#endif
