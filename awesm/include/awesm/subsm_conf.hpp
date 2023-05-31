//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_CONF_HPP
#define AWESM_SM_CONF_HPP

#include "transition_table.hpp"
#include "type_patterns.hpp"
#include "detail/constant.hpp"
#include "detail/tlu.hpp"
#include "detail/conf.hpp"

namespace awesm
{

template<class... Options>
struct subsm_conf_tpl
{
    template<class T>
    using context = subsm_conf_tpl<Options..., detail::options::context<T>>;

    using pretty_name = subsm_conf_tpl<Options..., detail::options::pretty_name>;

    template<class... EventFilters>
    using on_event = subsm_conf_tpl<Options..., detail::options::on_event<EventFilters...>>;

    using on_event_auto = subsm_conf_tpl<Options..., detail::options::on_event_auto>;

    using on_entry_any = subsm_conf_tpl<Options..., detail::options::on_entry_any>;

    using on_exit_any = subsm_conf_tpl<Options..., detail::options::on_exit_any>;

    template<class... Ts>
    using transition_tables = subsm_conf_tpl<Options..., detail::options::transition_tables<Ts...>>;
};

using subsm_conf = subsm_conf_tpl<>;

namespace detail
{
    template<class T>
    struct is_subsm_conf
    {
        static constexpr auto value = false;
    };

    template<class... Options>
    struct is_subsm_conf<subsm_conf_tpl<Options...>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_subsm_conf_v = is_subsm_conf<T>::value;
}

} //namespace

#endif
