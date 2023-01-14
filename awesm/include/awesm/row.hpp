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

namespace detail
{
    struct noop_t
    {
        template<class Context>
        void operator()(Context& /*ctx*/) const
        {
        }
    };

    struct null_guard_t
    {
        template<class Context>
        bool operator()(Context& /*ctx*/) const
        {
            return true;
        }
    };
}

constexpr inline auto noop = detail::noop_t{};
constexpr inline auto null_guard = detail::null_guard_t{};

template
<
    class SourceState,
    class Event,
    class TargetState,
    const auto& Action = noop,
    const auto& Guard = null_guard
>
struct row
{
    using source_state_type = SourceState;
    using event_type = Event;
    using target_state_type = TargetState;

    static constexpr const auto& get_action()
    {
        return Action;
    }

    static constexpr const auto& get_guard()
    {
        return Guard;
    }
};

} //namespace

#endif
