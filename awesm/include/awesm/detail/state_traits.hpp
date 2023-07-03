//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_TRAITS_HPP
#define AWESM_DETAIL_STATE_TRAITS_HPP

#include "submachine_fwd.hpp"
#include "overload_priority.hpp"
#include "conf.hpp"
#include "tlu.hpp"
#include "../type_patterns.hpp"
#include "../state_conf.hpp"
#include "../submachine_conf.hpp"
#include <type_traits>

namespace awesm::detail::state_traits
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
struct state_def_to_state<StateDef, Region, std::enable_if_t<is_submachine_conf_v<typename StateDef::conf>>>
{
    using type = submachine<StateDef, Region>;
};

template<class StateDef, class Region>
using state_def_to_state_t = typename state_def_to_state<StateDef, Region>::type;


//on_entry

template<class State>
constexpr auto requires_on_entry()
{
    return option_v<typename State::conf, option_id::on_entry_any>;
}


//on_exit

template<class State>
constexpr auto requires_on_exit()
{
    return option_v<typename State::conf, option_id::on_exit_any>;
}


//on_event

class has_on_event
{
private:
    template<class State, class Event>
    static constexpr auto has_impl(overload_priority::high /*unused*/) ->
        decltype(std::declval<State&>().on_event(std::declval<const Event&>()), bool())
    {
        return true;
    }

    template<class State, class Event>
    static constexpr bool has_impl(overload_priority::low /*unused*/)
    {
        return false;
    }

public:
    template<class State, class Event> //Template here for lazy evaluation
    static constexpr auto value = has_impl<State, Event>(overload_priority::probe);
};

struct matches_on_event_pattern
{
    template<class State, class Event> //Template here for lazy evaluation
    static constexpr auto value = matches_any_pattern_v
    <
        Event,
        option_t<typename State::conf, option_id::on_event>
    >;
};

template<class State, class Event>
constexpr auto requires_on_event_v = std::conditional_t
<
    option_v<typename State::conf, option_id::on_event_auto>,
    has_on_event,
    matches_on_event_pattern
>::template value<State, Event>;


//needs_unique_instance

template<class State>
struct needs_unique_instance
{
    static constexpr auto value = !(std::is_empty_v<State> && std::is_trivially_default_constructible_v<State>);
};

} //namespace

#endif
