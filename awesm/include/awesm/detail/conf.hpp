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
    X(before_entry,            constant<false>) \
    X(before_state_transition, constant<false>) \
    X(context,                 void) \
    X(get_pretty_name,         constant<false>) \
    X(on_entry,                constant<false>) \
    X(on_event,                type_list<>) \
    X(on_event_auto,           constant<false>) \
    X(on_exception,            constant<false>) \
    X(on_exit,                 constant<false>) \
    X(on_unprocessed,          constant<false>) \
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

} //namespace

#endif
