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

template<class... Options>
struct state_conf_tpl
{
    using pretty_name = state_conf_tpl<Options..., detail::options::pretty_name>;

    template<class... EventFilters>
    using on_event = state_conf_tpl<Options..., detail::options::on_event<EventFilters...>>;

    using on_event_auto = state_conf_tpl<Options..., detail::options::on_event_auto>;

    using on_entry_any = state_conf_tpl<Options..., detail::options::on_entry_any>;

    using on_exit_any = state_conf_tpl<Options..., detail::options::on_exit_any>;
};

using state_conf = state_conf_tpl<>;

} //namespace

#endif
