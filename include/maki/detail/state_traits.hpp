//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_TRAITS_HPP
#define MAKI_DETAIL_STATE_TRAITS_HPP

#include "submachine_fwd.hpp"
#include "overload_priority.hpp"
#include "tlu.hpp"
#include "../type_patterns.hpp"
#include "../state_conf.hpp"
#include "../submachine_conf.hpp"
#include <type_traits>

namespace maki::detail::state_traits
{

//is_submachine

template<class State>
struct is_submachine
{
    static constexpr auto value = false;
};

template<class Def, class ParentRegion>
struct is_submachine<submachine<Def, ParentRegion>>
{
    static constexpr auto value = true;
};

template<class State>
constexpr auto is_submachine_v = is_submachine<State>::value;


//state_def_to_state

template<class StateDef, class Region, class Enable = void>
struct state_def_to_state
{
    using type = StateDef;
};

template<class StateDef, class Region>
struct state_def_to_state<StateDef, Region, std::enable_if_t<is_submachine_conf_v<std::decay_t<decltype(StateDef::conf)>>>>
{
    using type = submachine<StateDef, Region>;
};

template<class StateDef, class Region>
using state_def_to_state_t = typename state_def_to_state<StateDef, Region>::type;


//on_event

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
        decltype(State::conf.event_actions),
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


//needs_unique_instance

template<class State>
struct needs_unique_instance
{
    static constexpr auto value = !(std::is_empty_v<State> && std::is_trivially_default_constructible_v<State>);
};

} //namespace

#endif
