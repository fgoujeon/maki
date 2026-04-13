//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_MOLD_IMPL_HPP
#define MAKI_STATE_MOLD_IMPL_HPP

#include "../context.hpp"
#include "tuple.hpp"
#include "mix.hpp"
#include <string_view>

namespace maki::detail
{

template
<
    class Context = void,
    class EntryActionTuple = tuple<>,
    class InternalActionTuple = tuple<>,
    class ExitActionTuple = tuple<>,
    class TransitionTableTuple = mix<>
>
struct state_mold_impl
{
    using context_type = Context;
    using internal_action_mix_type = InternalActionTuple;
    using transition_table_tuple_type = TransitionTableTuple;

    state_context_signature context_sig = state_context_signature::v;
    state_context_lifetime context_lifetime = state_context_lifetime::parent;
    EntryActionTuple entry_actions;
    InternalActionTuple internal_actions;
    ExitActionTuple exit_actions;
    std::string_view pretty_name;
    TransitionTableTuple transition_tables;
};

} //namespace

#endif
