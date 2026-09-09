//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MACHINE_OPERATION_HPP
#define MAKI_DETAIL_MACHINE_OPERATION_HPP

namespace maki::detail
{

enum class machine_operation: char
{
    start,
    stop,
    process_event
};

} //namespace

#endif
