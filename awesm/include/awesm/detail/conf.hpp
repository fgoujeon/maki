//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_CONF_HPP
#define AWESM_DETAIL_CONF_HPP

#include "tlu.hpp"
#include "type_tag.hpp"
#include "constant.hpp"
#include <type_traits>

namespace awesm::detail
{

enum class option_id
{
    after_state_transition,
    auto_start,
    before_entry,
    before_state_transition,
    context,
    get_pretty_name,
    on_entry,
    on_event,
    on_event_auto,
    on_exception,
    on_exit,
    on_unprocessed,
    run_to_completion,
    small_event_max_align,
    small_event_max_size,
    transition_tables,
};

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

//Get the type associated with the given ID
template<class Conf, option_id Id, class Default>
using get_option_t = typename tlu::find_t
<
    Conf,
    for_option_id<Id>::template has_id,
    type_tag<Default>
>::type;

template<class Conf, option_id Id, auto Default>
constexpr auto get_option_value = get_option_t<Conf, Id, constant<Default>>::value;

struct null_option_value
{
};

template<class Conf, option_id Id>
constexpr auto has_option_v = !std::is_same_v
<
    get_option_t<Conf, Id, null_option_value>,
    null_option_value
>;

} //namespace

#endif
