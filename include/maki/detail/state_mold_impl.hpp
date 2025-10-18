//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_MOLD_IMPL_HPP
#define MAKI_STATE_MOLD_IMPL_HPP

#include "../context.hpp"
#include "mix.hpp"
#include <string_view>

namespace maki::detail
{

template
<
    class Context = void,
    class EntryActionTuple = mix<>,
    class InternalActionTuple = mix<>,
    class ExitActionTuple = mix<>,
    class TransitionTableTuple = mix<>
>
struct state_mold_impl
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

} //namespace

#endif
