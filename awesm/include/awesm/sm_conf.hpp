//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_CONF_HPP
#define AWESM_SM_CONF_HPP

#include "subsm_conf.hpp"
#include "transition_table.hpp"
#include "pretty_name.hpp"
#include "detail/mix.hpp"

namespace awesm
{

namespace sm_opts
{
    namespace unsafe
    {
        struct disable_run_to_completion{};
    }

    template<class... EventPatterns>
    using on_event = state_opts::on_event<EventPatterns...>;

    struct after_state_transition{};
    struct before_entry{};
    struct before_state_transition{};
    struct disable_auto_start{};
    struct on_exception{};

    template<size_t Value>
    struct small_event_max_size
    {
        static constexpr size_t get_small_event_max_size()
        {
            return Value;
        }
    };

    template<size_t Value>
    struct small_event_max_align
    {
        static constexpr size_t get_small_event_max_alignment_requirement()
        {
            return Value;
        }
    };

    using get_pretty_name = detail::get_pretty_name_option;
}

template<auto TransitionTableFn, class Context, class... Options>
struct sm_conf
{
    using context_type = Context;
    using option_mix_type = detail::mix<Options...>;
    static constexpr auto transition_table_fn = TransitionTableFn;
};

} //namespace

#endif
