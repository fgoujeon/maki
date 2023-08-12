//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_SUBMACHINE_CONF_HPP
#define MAKI_SUBMACHINE_CONF_HPP

#include "transition_table.hpp"
#include "type_patterns.hpp"
#include "detail/constant.hpp"
#include "detail/tlu.hpp"
#include "detail/conf.hpp"

namespace maki
{

template<class... Options>
struct submachine_conf_tpl
{
    template<class T>
    using context = submachine_conf_tpl<Options..., detail::options::context<T>>;

    using pretty_name = submachine_conf_tpl<Options..., detail::options::pretty_name>;

    template<class... EventFilters>
    using on_event = submachine_conf_tpl<Options..., detail::options::on_event<EventFilters...>>;

    using on_event_auto = submachine_conf_tpl<Options..., detail::options::on_event_auto>;

    using on_entry_any = submachine_conf_tpl<Options..., detail::options::on_entry_any>;

    using on_exit_any = submachine_conf_tpl<Options..., detail::options::on_exit_any>;

    template<class... Ts>
    using transition_tables = submachine_conf_tpl<Options..., detail::options::transition_tables<Ts...>>;
};

using submachine_conf = submachine_conf_tpl<>;

namespace detail
{
    template<class T>
    struct is_submachine_conf
    {
        static constexpr auto value = false;
    };

    template<class... Options>
    struct is_submachine_conf<submachine_conf_tpl<Options...>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_submachine_conf_v = is_submachine_conf<T>::value;
}

} //namespace

#endif
