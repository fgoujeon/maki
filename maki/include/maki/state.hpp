//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_HPP
#define MAKI_STATE_HPP

#include "noop.hpp"
#include <string_view>

namespace maki
{

template<class OnEntry = noop_t, class OnEvent = noop_t, class OnExit = noop_t>
struct state
{
    std::string_view pretty_name;
    OnEntry on_entry = noop;
    OnEvent on_event = noop;
    OnExit on_exit = noop;

    constexpr auto set_pretty_name(const std::string_view value) const
    {
        return state{value, on_entry, on_event, on_exit};
    }

    template<class Value>
    constexpr auto set_on_entry(const Value& value) const
    {
        return state{pretty_name, value, on_event, on_exit};
    }

    template<class Value>
    constexpr auto set_on_event(const Value& value) const
    {
        return state{pretty_name, on_entry, value, on_exit};
    }

    template<class Value>
    constexpr auto set_on_exit(const Value& value) const
    {
        return state{pretty_name, on_entry, on_event, value};
    }
};

inline constexpr auto state_c = state<>{};

} //namespace

#endif
