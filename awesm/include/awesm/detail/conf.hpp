//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_CONF_HPP
#define AWESM_DETAIL_CONF_HPP

#include "tlu.hpp"
#include "type.hpp"
#include "type_list.hpp"
#include "constant.hpp"
#include <type_traits>

namespace awesm::detail
{

static constexpr auto small_event_default_max_size = 16;
static constexpr auto small_event_default_max_align = 8;

#define AWESM_DETAIL_OPTIONS \
    X(after_state_transition,  constant<false>) \
    X(auto_start,              constant<true>) \
    X(before_state_transition, constant<false>) \
    X(context,                 void) \
    X(events,                  type_list<>) \
    X(on_entry_any,            constant<false>) \
    X(on_event,                type_list<>) \
    X(on_event_auto,           constant<false>) \
    X(on_exception,            constant<false>) \
    X(on_exit_any,             constant<false>) \
    X(on_unprocessed,          constant<false>) \
    X(pretty_name,             constant<false>) \
    X(run_to_completion,       constant<true>) \
    X(small_event_max_align,   constant<small_event_default_max_align>) \
    X(small_event_max_size,    constant<small_event_default_max_size>) \
    X(transition_tables,       type_list<>)

enum class option_id
{
#define X(id, default_type) id, //NOLINT
    AWESM_DETAIL_OPTIONS
#undef X
};

template<option_id Id>
struct default_option;

#define X(id, default_type) /*NOLINT*/ \
    template<> \
    struct default_option<option_id::id> \
    { \
        using type = default_type; \
    };
AWESM_DETAIL_OPTIONS
#undef X

template<option_id Id>
using default_option_t = typename default_option<Id>::type;

#undef AWESM_DETAIL_OPTIONS

template<option_id Id, class T>
struct conf_element
{
    using type = T;
    static constexpr auto opt_id = Id;
};

template<option_id Id>
struct for_option_id
{
    template<class Element>
    struct has_id
    {
        static constexpr auto value = Id == Element::opt_id;
    };
};

//Get the type associated with the given option ID
template<class Conf, option_id Id>
using option_t = typename tlu::find_t
<
    Conf,
    for_option_id<Id>::template has_id,
    type_t<default_option_t<Id>>
>::type;

template<class Conf, option_id Id, class Default>
using option_or_t = typename tlu::find_t
<
    Conf,
    for_option_id<Id>::template has_id,
    type_t<Default>
>::type;

//Get the value associated with the given option ID
template<class Conf, option_id Id>
constexpr auto option_v = option_t<Conf, Id>::value;

//All options, sorted alphabetically
namespace options
{
    using after_state_transition = conf_element
    <
        option_id::after_state_transition,
        constant<true>
    >;

    using no_auto_start = conf_element<option_id::auto_start, constant<false>>;

    using before_state_transition = conf_element
    <
        option_id::before_state_transition,
        constant<true>
    >;

    template<class T>
    using context = conf_element<option_id::context, T>;

    using no_run_to_completion = conf_element
    <
        option_id::run_to_completion,
        constant<false>
    >;

    using pretty_name = conf_element
    <
        option_id::pretty_name,
        constant<true>
    >;

    using on_exception = conf_element<option_id::on_exception, constant<true>>;

    using on_unprocessed = conf_element<option_id::on_unprocessed, constant<true>>;

    template<std::size_t Value>
    using small_event_max_align = conf_element
    <
        option_id::small_event_max_align,
        constant<Value>
    >;

    template<std::size_t Value>
    using small_event_max_size = conf_element
    <
        option_id::small_event_max_size,
        constant<Value>
    >;

    template<class... EventFilters>
    using on_event = conf_element<option_id::on_event, type_list<EventFilters...>>;

    using on_event_auto = conf_element<option_id::on_event_auto, constant<true>>;

    using on_entry_any = conf_element<option_id::on_entry_any, constant<true>>;

    using on_exit_any = conf_element<option_id::on_exit_any, constant<true>>;

    template<class... Ts>
    using transition_tables = conf_element<option_id::transition_tables, type_list<Ts...>>;
}

} //namespace

#endif
