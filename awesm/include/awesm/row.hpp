//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_ROW_HPP
#define AWESM_ROW_HPP

#include "null.hpp"

namespace awesm
{

inline void null_action()
{
}

inline bool null_guard()
{
    return true;
}

template
<
    class SourceState,
    class Event,
    class TargetState,
    const auto& Action = null_action,
    const auto& Guard = null_guard
>
struct row
{
    using source_state_type = SourceState;
    using event_type = Event;
    using target_state_type = TargetState;
    static constexpr const auto& action = Action;
    static constexpr const auto& guard = Guard;
};

} //namespace

#endif
