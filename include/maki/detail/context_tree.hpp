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

    auto& context()
    {
        return ctx_holder_.get_deep();
    }

    const auto& context() const
    {
        return ctx_holder_.get_deep();
    }

    auto& opt_context()
    {
        return ctx_holder_.get();
    }

    const auto& opt_context() const
    {
        return ctx_holder_.get();
    }

    template<class StateMoldPath2>
    auto& node_at()
    {
        if constexpr (std::is_same_v<StateMoldPath, StateMoldPath2>)
        {
            return *this;
        }
        else
        {
            return child_toward<StateMoldPath2>().template node_at<StateMoldPath2>();
        }
    }

    template<class StateMoldPath2>
    const auto& node_at() const
    {
        if constexpr (std::is_same_v<StateMoldPath, StateMoldPath2>)
        {
            return *this;
        }
        else
        {
            return child_toward<StateMoldPath2>().template node_at<StateMoldPath2>();
        }
    }

    template<class ParentContext>
    void emplace_context(
        machine<MachineConfHolder>& mach,
        ParentContext& parent_ctx)
    {
        auto& ctx = ctx_holder_.emplace(mach, parent_ctx);

        /*
        Also emplace subcontexts with parent lifetime, which depend on the context we just emplaced.
        */
        tlu::apply_t<children_emplace_context_with_parent_lifetime, child_types>::call(*this, mach, ctx);
    }

    template<class ParentContext>
    void emplace_context_with_parent_lifetime(
        machine<MachineConfHolder>& mach,
        ParentContext& parent_ctx)
    {
        if constexpr (ctx_lifetime == state_context_lifetime::parent)
        {
            emplace_context(mach, parent_ctx);
        }
    }

    template<class StateMoldPath2, class ParentContext>
    void emplace_context_at(
        machine<MachineConfHolder>& mach,
        ParentContext& parent_ctx)
    {
        if constexpr (std::is_same_v<StateMoldPath, StateMoldPath2>)
        {
            emplace_context(mach, parent_ctx);
        }
        else
        {
            child_toward<StateMoldPath2>().template emplace_context_at<StateMoldPath2>(mach, ctx_holder_.get_deep());
        }
    }

    void reset_context()
    {
        /*
        First reset subcontexts with parent lifetime, which depend on the context we're about to
        reset.
        */
        tlu::apply_t<children_reset_context_with_parent_lifetime, child_types>::call(*this);

        ctx_holder_.reset();
    }

    void reset_context_with_parent_lifetime()
    {
        if constexpr (ctx_lifetime == state_context_lifetime::parent)
        {
            reset_context();
        }
    }

    template<class StateMoldPath2>
    void reset_context_at()
    {
        if constexpr (std::is_same_v<StateMoldPath, StateMoldPath2>)
        {
            reset_context();
        }
        else
        {
            child_toward<StateMoldPath2>().template reset_context_at<StateMoldPath2>();
        }
    }

private:
    static constexpr const auto& state_mold =
        machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;

    using state_mold_impl_type = std::decay_t<decltype(impl_of(state_mold))>;

    using context_type = typename state_mold_impl_type::context_type;

    static constexpr auto ctx_lifetime = impl_of(state_mold).context_lifetime;

    static constexpr auto ctx_storage =
        ctx_lifetime == state_context_lifetime::parent
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

    template<class... Child>
    struct children_emplace_context_with_parent_lifetime
    {
        static void call(
            context_tree_node& self,
            machine<MachineConfHolder>& mach,
            context_type& ctx)
        {
            (get<Child>(self.children_).emplace_context_with_parent_lifetime(mach, ctx), ...);
        }
    };

    template<class... Child>
    struct children_reset_context_with_parent_lifetime
    {
        static void call(context_tree_node& self)
        {
            (get<Child>(self.children_).reset_context_with_parent_lifetime(), ...);
        }
    };

    template<class StateMoldPath2>
    auto& child_toward()
    {
        using child_t = tlu::find_if_t<
            child_types,
            context_tree_detail::node_has_state_mold_path_starting_with<
                StateMoldPath2>::template inner>;
        return get<child_t>(children_);
    }

    template<class StateMoldPath2>
    const auto& child_toward() const
    {
        using child_t = tlu::find_if_t<
            child_types,
            context_tree_detail::node_has_state_mold_path_starting_with<
                StateMoldPath2>::template inner>;
        return get<child_t>(children_);
    }

    context_holder<context_type, ctx_storage, ctx_sig> ctx_holder_;

    children_mix children_;
};

template<class MachineConfHolder>
class context_tree
{
public:
    template<class... Args>
    context_tree(machine<MachineConfHolder>& mach, Args&&... args):
        mach_(mach),
        impl_(mach, std::forward<Args>(args)...)
    {
    }

    context_tree(const context_tree&) = delete;
    context_tree(context_tree&&) = delete;
    context_tree& operator=(const context_tree&) = delete;
    context_tree& operator=(context_tree&&) = delete;

    template<class StateMoldPath>
    auto& context_at()
    {
        return impl_.template node_at<StateMoldPath>().context();
    }

    template<class StateMoldPath>
    const auto& context_at() const
    {
        return impl_.template node_at<StateMoldPath>().context();
    }

    template<class StateMoldPath>
    auto& opt_context_at()
    {
        return impl_.template node_at<StateMoldPath>().opt_context();
    }

    template<class StateMoldPath>
    const auto& opt_context_at() const
    {
        return impl_.template node_at<StateMoldPath>().opt_context();
    }

    template<class StateMoldPath>
    void emplace_context_at()
    {
        impl_.template emplace_context_at<StateMoldPath>(mach_, impl_.context());
    }

    template<class StateMoldPath>
    void reset_context_at()
    {
        impl_.template reset_context_at<StateMoldPath>();
    }

private:
    machine<MachineConfHolder>& mach_;
    context_tree_node<MachineConfHolder, iseq<>, context_storage::plain> impl_;
};

} // namespace maki::detail

#endif
