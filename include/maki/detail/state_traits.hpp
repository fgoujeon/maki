//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_TRAITS_HPP
#define MAKI_DETAIL_STATE_TRAITS_HPP

#include "submachine_fwd.hpp"
#include "simple_state_fwd.hpp"
#include "tlu.hpp"
#include "../type_patterns.hpp"
#include "../submachine_conf.hpp"
#include <type_traits>

namespace maki::detail::state_traits
{

//state_conf_to_state

template<const auto& StateConf, class Region, class Enable = void>
struct state_conf_to_state
{
    using type = simple_state<StateConf>;
};

template<const auto& StateConf, class Region>
struct state_conf_to_state<StateConf, Region, std::enable_if_t<is_submachine_conf_v<std::decay_t<decltype(StateConf)>>>>
{
    using type = submachine<StateConf, Region>;
};

template<const auto& StateConf, class Region>
using state_conf_to_state_t = typename state_conf_to_state<StateConf, Region>::type;


//requires_on_event

template<class State, class Event>
class requires_on_event
{
private:
    struct not_found{};

    template<class Action>
    struct takes_event
    {
        static constexpr auto value = matches_pattern_v
        <
            Event,
            typename Action::event_type_filter
        >;
    };

    using first_matching_action_type = tlu::find_t
    <
        decltype(State::conf.internal_actions_),
        takes_event,
        not_found
    >;

public:
    static constexpr bool value = !std::is_same_v
    <
        first_matching_action_type,
        not_found
    >;
};

template<class State, class Event>
constexpr auto requires_on_event_v = requires_on_event<State, Event>::value;

} //namespace

#endif
