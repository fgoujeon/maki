// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONTEXT_TREE_HPP
#define MAKI_DETAIL_CONTEXT_TREE_HPP

#include "../context.hpp"
#include "context_holder.hpp"
#include "context_storage.hpp"
#include "friendly_impl.hpp"
#include "iseq.hpp"
#include "machine_conf_tree.hpp"
#include "machine_fwd.hpp"
#include "mix.hpp"
#include "tlu/apply.hpp"
#include "tlu/find_if.hpp"
#include "tlu/map.hpp"
#include "tlu/push_back.hpp"
#include "transition_table_digest.hpp"
#include "type_list.hpp"
#include <type_traits>

namespace maki::detail
{

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentStorage>
class context_tree_node;

namespace context_tree_detail
{
    template<class MachineConfHolder, context_storage ParentStorage>
    struct node_holder
    {
        template<class StateMoldPath>
        using type =
            context_tree_node<MachineConfHolder, StateMoldPath, ParentStorage>;
    };

    template<class MachineConfHolder, class ParentStateMoldPath>
    struct add_contextful_state_mold_paths_of_transition_table;

    template<class MachineConfHolder, class TransitionTablePath>
    struct add_contextful_state_mold_paths_of_transition
    {
        template<class Paths, int TransitionIndex>
        struct inner
        {
            using state_mold_path =
                iseq_push_back_t<TransitionTablePath, TransitionIndex>;

            using state_mold_type = machine_conf_tree::
                node_at_path_t<MachineConfHolder, state_mold_path>;

            using state_mold_transition_table_tuple_type = typename impl_of_t<
                state_mold_type>::transition_table_tuple_type;

            using paths_plus_current = tlu::push_back_t<Paths, state_mold_path>;

            using paths_plus_children = iseq_left_fold_t<
                add_contextful_state_mold_paths_of_transition_table<
                    MachineConfHolder,
                    state_mold_path>::template inner_t,
                Paths,
                linear_iseq_t<state_mold_transition_table_tuple_type::size>>;

            using type = std::conditional_t<
                std::is_void_v<typename state_mold_type::context_type>,
                paths_plus_children,
                paths_plus_current>;
        };

        template<class Paths, int TransitionIndex>
        using inner_t = typename inner<Paths, TransitionIndex>::type;
    };

    template<class MachineConfHolder, class ParentStateMoldPath>
    struct add_contextful_state_mold_paths_of_transition_table
    {
        template<class Paths, int TransitionTableIndex>
        struct inner
        {
            using transition_table_path =
                iseq_push_back_t<ParentStateMoldPath, TransitionTableIndex>;

            using state_mold_iseq = typename transition_table_digest<
                MachineConfHolder,
                transition_table_path>::stt_mold_ids;

            using type = iseq_left_fold_t<
                add_contextful_state_mold_paths_of_transition<
                    MachineConfHolder,
                    transition_table_path>::template inner_t,
                Paths,
                state_mold_iseq>;
        };

        template<class Paths, int TransitionTableIndex>
        using inner_t = typename inner<Paths, TransitionTableIndex>::type;
    };

    template<class Path>
    struct node_has_state_mold_path_starting_with
    {
        template<class Node>
        struct inner
        {
            static constexpr auto value =
                iseq_starts_with_v<Path, typename Node::state_mold_path>;
        };
    };
} // namespace context_tree_detail

template<
    class MachineConfHolder,
    class StateMoldPath,
    context_storage ParentStorage>
class context_tree_node
{
public:
    using state_mold_path = StateMoldPath;

    template<class... Args>
    context_tree_node(machine<MachineConfHolder>& mach, Args&&... args):
        ctx_holder_(mach, std::forward<Args>(args)...),
        children_(mix_uniform_construct, mach, ctx_holder_.get())
    {
    }

    template<class StateMoldPath2>
    auto& context_holder_at()
    {
        if constexpr (std::is_same_v<StateMoldPath, StateMoldPath2>)
        {
            return ctx_holder_;
        }
        else
        {
            using child_t = tlu::find_if_t<
                child_types,
                context_tree_detail::node_has_state_mold_path_starting_with<
                    StateMoldPath2>::template inner>;
            return get<child_t>(children_)
                .template context_holder_at<StateMoldPath2>();
        }
    }

    template<class StateMoldPath2>
    auto& context_at()
    {
        if constexpr (std::is_same_v<StateMoldPath, StateMoldPath2>)
        {
            return ctx_holder_.get();
        }
        else
        {
            using child_t = tlu::find_if_t<
                child_types,
                context_tree_detail::node_has_state_mold_path_starting_with<
                    StateMoldPath2>::template inner>;
            return get<child_t>(children_)
                .template context_at<StateMoldPath2>();
        }
    }

    template<class StateMoldPath2>
    const auto& context_at() const
    {
        if constexpr (std::is_same_v<StateMoldPath, StateMoldPath2>)
        {
            return ctx_holder_.get();
        }
        else
        {
            using child_t = tlu::find_if_t<
                child_types,
                context_tree_detail::node_has_state_mold_path_starting_with<
                    StateMoldPath2>::template inner>;
            return get<child_t>(children_)
                .template context_at<StateMoldPath2>();
        }
    }

private:
    static constexpr const auto& state_mold =
        machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;

    using state_mold_impl_type = std::decay_t<decltype(impl_of(state_mold))>;

    using context_type = typename state_mold_impl_type::context_type;

    static constexpr auto ctx_storage =
        impl_of(state_mold).context_lifetime == state_context_lifetime::parent
        ? ParentStorage
        : context_storage::optional;

    static constexpr auto ctx_sig = impl_of(state_mold).context_sig;

    using transition_table_tuple_type =
        typename state_mold_impl_type::transition_table_tuple_type;

    using contextful_state_mold_paths = iseq_left_fold_t<
        context_tree_detail::
            add_contextful_state_mold_paths_of_transition_table<
                MachineConfHolder,
                StateMoldPath>::template inner_t,
        type_list_t<>,
        linear_iseq_t<transition_table_tuple_type::size>>;

    using child_types = tlu::map_t<
        context_tree_detail::node_holder<MachineConfHolder, ctx_storage>::
            template type,
        contextful_state_mold_paths>;

    using children_mix = tlu::apply_t<mix, child_types>;

    context_holder<context_type, ctx_storage, ctx_sig> ctx_holder_;

    children_mix children_;
};

template<class MachineConfHolder>
using context_tree =
    context_tree_node<MachineConfHolder, iseq<>, context_storage::plain>;

} // namespace maki::detail

#endif
