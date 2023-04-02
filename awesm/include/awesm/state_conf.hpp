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
#include "detail/type.hpp"
#include "detail/conf.hpp"

namespace awesm
{

namespace state_opts
{
    template<bool Enable = true>
    using on_entry = detail::conf_element<detail::option_id::on_entry, detail::constant<Enable>>;

    template<class... EventFilters>
    using on_event = detail::conf_element<detail::option_id::on_event, detail::type_list<EventFilters...>>;

    using on_event_auto = detail::conf_element<detail::option_id::on_event_auto, detail::constant<true>>;

    template<bool Enable = true>
    using on_exit = detail::conf_element<detail::option_id::on_exit, detail::constant<Enable>>;

    using pretty_name = detail::conf_element<detail::option_id::pretty_name, detail::constant<true>>;
}

template<class... Options>
struct state_conf_tpl
{
    using pretty_name = state_conf_tpl<Options..., state_opts::pretty_name>;

    template<class... EventFilters>
    using on_event = state_conf_tpl<Options..., state_opts::on_event<EventFilters...>>;

    using on_event_auto = state_conf_tpl<Options..., state_opts::on_event_auto>;

    template<bool Enable = true>
    using on_entry = state_conf_tpl<Options..., state_opts::on_entry<Enable>>;

    template<bool Enable = true>
    using on_exit = state_conf_tpl<Options..., state_opts::on_exit<Enable>>;

    static constexpr auto is_composite = false;
};

using state_conf = state_conf_tpl<>;

} //namespace

#endif
