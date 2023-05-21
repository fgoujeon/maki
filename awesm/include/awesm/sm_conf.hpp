//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_ROOT_SM_CONF_HPP
#define AWESM_ROOT_SM_CONF_HPP

#include "transition_table.hpp"
#include "type_filters.hpp"
#include "detail/constant.hpp"
#include "detail/tlu.hpp"
#include "detail/conf.hpp"

namespace awesm
{

template<class... Options>
struct sm_conf_tpl
{
    using after_state_transition = sm_conf_tpl<Options..., detail::options::after_state_transition>;

    using no_auto_start = sm_conf_tpl<Options..., detail::options::no_auto_start>;

    using before_entry = sm_conf_tpl<Options..., detail::options::before_entry>;

    using before_state_transition = sm_conf_tpl<Options..., detail::options::before_state_transition>;

    template<class T>
    using context = sm_conf_tpl<Options..., detail::options::context<T>>;

    using no_run_to_completion = sm_conf_tpl<Options..., detail::options::no_run_to_completion>;

    using pretty_name = sm_conf_tpl<Options..., detail::options::pretty_name>;

    using on_exception = sm_conf_tpl<Options..., detail::options::on_exception>;

    using on_unprocessed = sm_conf_tpl<Options..., detail::options::on_unprocessed>;

    template<std::size_t Value>
    using small_event_max_align = sm_conf_tpl<Options..., detail::options::small_event_max_align<Value>>;

    template<std::size_t Value>
    using small_event_max_size = sm_conf_tpl<Options..., detail::options::small_event_max_size<Value>>;

    template<class... EventFilters>
    using on_event = sm_conf_tpl<Options..., detail::options::on_event<EventFilters...>>;

    using on_event_auto = sm_conf_tpl<Options..., detail::options::on_event_auto>;

    using on_entry_any = sm_conf_tpl<Options..., detail::options::on_entry_any>;

    using on_exit_any = sm_conf_tpl<Options..., detail::options::on_exit_any>;

    template<class... Ts>
    using transition_tables = sm_conf_tpl<Options..., detail::options::transition_tables<Ts...>>;

    static constexpr auto is_root = true;
};

using sm_conf = sm_conf_tpl<>;

namespace detail
{
    template<class T>
    struct is_root_sm_conf
    {
        static constexpr auto value = false;
    };

    template<class... Options>
    struct is_root_sm_conf<sm_conf_tpl<Options...>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_root_sm_conf_v = is_root_sm_conf<T>::value;
}

} //namespace

#endif
