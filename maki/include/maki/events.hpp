//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::events namespace and its types
*/

#ifndef MAKI_EVENTS_HPP
#define MAKI_EVENTS_HPP

#include <exception>

/**
@brief Some predefined events emitted by Maki itself
*/
namespace maki::events
{

/**
@brief Default event given to `machine::start()`.
*/
struct start{};

/**
@brief Default event given to `machine::stop()`.
*/
struct stop{};

/**
@brief Event sent by the default exception handler of @ref machine whenever it
catches an exception.
*/
struct exception
{
    /**
    @brief Pointer to the caught exception
    */
    std::exception_ptr eptr;
};

} //namespace

#endif
