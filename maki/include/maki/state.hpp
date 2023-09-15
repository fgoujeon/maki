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
    std::string_view name;
    OnEntry on_entry = noop;
    OnEvent on_event = noop;
    OnExit on_exit = noop;

    constexpr auto set_name(const std::string_view value) const
    {
        return make_state(value, on_entry, on_event, on_exit);
    }

    template<class Value>
    constexpr auto set_on_entry(const Value& value) const
    {
        return make_state(name, value, on_event, on_exit);
    }

    template<class Value>
    constexpr auto set_on_event(const Value& value) const
    {
        return make_state(name, on_entry, value, on_exit);
    }

    template<class Value>
    constexpr auto set_on_exit(const Value& value) const
    {
        return make_state(name, on_entry, on_event, value);
    }
};

template<class... Args>
auto make_state(Args&&... args)
{
    return state<std::decay_t<Args>...>(std::forward<Args>(args)...);
}

inline constexpr auto state_c = state<>{};

} //namespace

#endif
