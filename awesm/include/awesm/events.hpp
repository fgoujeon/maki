//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_EVENTS_HPP
#define AWESM_EVENTS_HPP

#include <exception>

namespace awesm::events
{

/**
@brief Default event given to `sm::start()`.
*/
struct start{};

/**
@brief Default event given to `sm::stop()`.
*/
struct stop{};

/**
@brief Event sent by the default exception handler of @ref sm whenever it
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
