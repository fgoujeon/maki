//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_TRAITS_HPP
#define AWESM_DETAIL_STATE_TRAITS_HPP

#include "subsm_fwd.hpp"
#include "overload_priority.hpp"
#include "alternative.hpp"
#include "tlu.hpp"
#include "../type_filters.hpp"
#include "../state_conf.hpp"
#include "../sm_conf.hpp"

namespace awesm::detail::state_traits
{

//wrap

template<class State, class Region, class Enable = void>
struct wrap
{
    using type = State;
};

template<class State, class Region>
struct wrap<State, Region, std::enable_if_t<State::conf::is_composite>>
{
    using type = subsm<State, Region>;
};

template<class State, class Region>
using wrap_t = typename wrap<State, Region>::type;


//on_entry

static_assert(static_cast<int>(sm_option::on_entry) == static_cast<int>(state_option::on_entry));

template<class State>
constexpr auto requires_on_entry_v =
    tlu::get_f_t<typename State::conf, sm_option::on_entry>::value
;


//on_exit

static_assert(static_cast<int>(sm_option::on_exit) == static_cast<int>(state_option::on_exit));

template<class State>
constexpr auto requires_on_exit_v =
    tlu::get_f_t<typename State::conf, sm_option::on_exit>::value
;


//on_event

static_assert(static_cast<int>(sm_option::on_event) == static_cast<int>(state_option::on_event));
static_assert(static_cast<int>(sm_option::on_event_auto) == static_cast<int>(state_option::on_event_auto));

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

struct matches_on_event_filter
{
    template<class State, class Event> //Template here for lazy evaluation
    static constexpr auto value = matches_any_filter_v
    <
        Event,
        tlu::get_f_t<typename State::conf, sm_option::on_event>
    >;
};

template<class State, class Event>
constexpr auto requires_on_event_v = alternative_t
<
    tlu::get_f_t<typename State::conf, sm_option::on_event_auto>::value,
    has_on_event,
    matches_on_event_filter
>::template value<State, Event>;

} //namespace

#endif
