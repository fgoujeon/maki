//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_COMPOSITE_STATE_CONF_HPP
#define AWESM_COMPOSITE_STATE_CONF_HPP

#include "state_conf.hpp"
#include "pretty_name.hpp"
#include "detail/mix.hpp"

namespace awesm
{

namespace subsm_opts
{
    using on_entry_any = state_opts::on_entry_any;

    template<class... EventFilters>
    using on_event = state_opts::on_event<EventFilters...>;

    using on_event_any = state_opts::on_event_any;

    using on_exit_any = state_opts::on_exit_any;

    using get_pretty_name = detail::get_pretty_name_option;

    template<class T>
    struct context
    {
        using context_type = T;
    };
}

template<auto TransitionTableFn, class... Options>
struct subsm_conf
{
    using option_mix_type = detail::mix<Options...>;
    static constexpr auto is_composite = true;
    static constexpr auto transition_table_fn = TransitionTableFn;
};

} //namespace

#endif
