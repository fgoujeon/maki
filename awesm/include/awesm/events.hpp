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

//Default event sent by sm::start()
struct start{};

//Default event sent by sm::stop()
struct stop{};

//Event sent by the default on_exception policy of sm whenever it catches an
//exception
struct exception
{
    std::exception_ptr eptr;
};

} //namespace

#endif
