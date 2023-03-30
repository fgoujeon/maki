//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_WHATEVER_HPP
#define AWESM_WHATEVER_HPP

namespace awesm
{

/**
@defgroup Utility
Some general-purpose utility types and functions that can be handy when using
the AweSM API.
*/

/**
@ingroup Utility
An empty struct with a generic unary constructor that does nothing.
It is typically used in function parameter lists for parameters whose type and
value are unused.

For example, here is an action that only needs the \ref sm and the context, but
doesn't care about the event:
```cpp
template<class Sm>
void some_action(Sm& sm, context& ctx, awesm::whatever event)
{
    //...
}
```

This is a slightly more concise way of writing this:
```cpp
template<class Sm, class Event>
void some_action(Sm& sm, context& ctx, const Event& event)
{
    //...
}
```
*/
struct whatever
{
    /**
    @brief Does nothing.
    */
    template<class T>
    constexpr whatever(const T& /*unused*/)
    {
    }
};

} //namespace

#endif
