//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_STATE_CONF_HPP
#define AWESM_STATE_CONF_HPP

#include "type_filters.hpp"
#include "detail/constant.hpp"
#include "detail/tlu.hpp"
#include "detail/type_tag.hpp"
#include "detail/conf.hpp"

namespace awesm
{

namespace state_opts
{
    template<bool Enable = true>
    using on_entry = detail::conf_element<detail::option_id::on_entry, detail::constant<Enable>>;

    template<class... EventFilters>
    using on_event = detail::conf_element<detail::option_id::on_event, detail::tlu::type_list<EventFilters...>>;

    using on_event_auto = detail::conf_element<detail::option_id::on_event_auto, detail::constant<true>>;

    template<bool Enable = true>
    using on_exit = detail::conf_element<detail::option_id::on_exit, detail::constant<Enable>>;

    template<bool Enable = true>
    using get_pretty_name = detail::conf_element<detail::option_id::get_pretty_name, detail::constant<Enable>>;
}

template<class... Options>
struct state_conf
{
    static constexpr auto is_composite = false;
};

} //namespace

#endif
