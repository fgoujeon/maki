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

#include "../context.hpp"
#include "mix.hpp"
#include <string_view>

namespace maki
{

namespace detail
{
    template
    <
        class Context = void,
        class EntryActionTuple = mix<>,
        class InternalActionTuple = mix<>,
        class ExitActionTuple = mix<>,
        class TransitionTableTuple = mix<>
    >
    struct state_mold_option_set
    {
        using context_type = Context;
        using internal_action_mix_type = InternalActionTuple;

        state_context_signature context_sig = state_context_signature::v;
        EntryActionTuple entry_actions;
        InternalActionTuple internal_actions;
        ExitActionTuple exit_actions;
        std::string_view pretty_name;
        TransitionTableTuple transition_tables;
    };
}

#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class OptionSet = detail::state_mold_option_set<>>
#endif
class state_mold;

namespace detail
{
    template<class T>
    struct is_state_mold
    {
        static constexpr auto value = false;
    };

    template<class OptionSet>
    struct is_state_mold<state_mold<OptionSet>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr bool is_state_mold_v = is_state_mold<T>::value;
}

} //namespace

#endif
