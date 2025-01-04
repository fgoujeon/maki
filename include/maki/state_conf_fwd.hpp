//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::state_conf struct template
*/

#ifndef MAKI_STATE_CONF_FWD_HPP
#define MAKI_STATE_CONF_FWD_HPP

#include "detail/context_signature.hpp"
#include "detail/tuple.hpp"
#include <string_view>

namespace maki
{

namespace detail
{
    template
    <
        class Context = void,
        class EntryActionTuple = detail::tuple<>,
        class InternalActionTuple = detail::tuple<>,
        class ExitActionTuple = detail::tuple<>,
        class TransitionTableTuple = detail::tuple<>
    >
    struct state_conf_option_set
    {
        using context_type = Context;

        context_signature context_sig = context_signature::v;
        EntryActionTuple entry_actions;
        InternalActionTuple internal_actions;
        ExitActionTuple exit_actions;
        std::string_view pretty_name;
        TransitionTableTuple transition_tables;
    };
}

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief State configuration
*/
template<IMPLEMENTATION_DETAIL>
#else
template<class OptionSet = detail::state_conf_option_set<>>
#endif
class state_conf;

} //namespace

#endif
