//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Forward-declares the maki::state_mold struct template
*/

#ifndef MAKI_STATE_MOLD_FWD_HPP
#define MAKI_STATE_MOLD_FWD_HPP

#include "../event_set.hpp"
#include "../context.hpp"
#include "mix.hpp"
#include "overload.hpp"
#include <string_view>

namespace maki
{

namespace detail
{
    template
    <
        class Context = void,
        class EntryActionTuple = mix<>,
        class InternalActionEventSetImpl = detail::type_set_impls::inclusion_list<>,
        class InternalActionOverload = overload<>,
        class ExitActionTuple = mix<>,
        class TransitionTableTuple = mix<>
    >
    struct state_mold_option_set
    {
        using context_type = Context;
        using internal_action_event_set_impl_type = InternalActionEventSetImpl;

        state_context_signature context_sig = state_context_signature::v;
        EntryActionTuple entry_actions;
        InternalActionOverload internal_actions;
        ExitActionTuple exit_actions;
        std::string_view pretty_name;
        TransitionTableTuple transition_tables;
    };
}

} //namespace

#endif
