//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_UNUSED_HPP
#define FGFSM_UNUSED_HPP

namespace fgfsm
{

/*
A type for ignored arguments.

Instead of writing this:
    template<class A, class B>
    void f(const A&, const B&)
    {
        //A and B aren't used...
    }

User can write this:
    void f(unused, unused)
    {
    }

It's shorter and clearer.
*/
struct unused
{
    template<class T>
    unused(T&&)
    {
    }
};

} //namespace

#endif
