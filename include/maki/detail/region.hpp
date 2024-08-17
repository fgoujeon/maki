//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_HPP
#define MAKI_DETAIL_REGION_HPP

#include "path_of.hpp"
#include "state_traits.hpp"
#include "call_member.hpp"
#include "transition_table_digest.hpp"
#include "transition_table_filters.hpp"
#include "state_type_list_filters.hpp"
#include "tlu/apply.hpp"
#include "tlu/empty.hpp"
#include "tlu/find.hpp"
#include "tlu/find_if.hpp"
#include "tlu/front.hpp"
#include "tlu/push_back.hpp"
#include "same_ref.hpp"
#include "maybe_bool_util.hpp"
#include "tuple.hpp"
#include "constant.hpp"
#include "../null.hpp"
#include "../cref_constant.hpp"
#include "../submachine_conf.hpp"
#include "../state_confs.hpp"
#include "../transition_table.hpp"
#include <type_traits>

namespace maki::detail
{

namespace region_detail
{
    inline constexpr auto stopped_state_index = -1;

    template<class StateList, class State>
    struct find_state
    {
        static constexpr auto value = tlu::find_v<StateList, State>;
    };

    template<class StateList, class Region>
    struct find_state<StateList, state_traits::state_id_to_state<&state_confs::stopped, Region>>
    {
        static constexpr auto value = stopped_state_index;
    };

    template<class StateList, class State>
    inline constexpr auto find_state_v = find_state<StateList, State>::value;

    template<class StateList, auto StateId>
    struct find_state_from_id
    {
        static constexpr auto value = tlu::find_if_v<StateList, state_traits::for_id<StateId>::template has_id>;
    };

    template<class StateList>
    struct find_state_from_id<StateList, &state_confs::stopped>
    {
        static constexpr auto value = stopped_state_index;
    };

    template<class StateList, auto StateId>
    inline constexpr auto find_state_from_id_v = find_state_from_id<StateList, StateId>::value;
}

template<class ParentSm, int Index>
class region
{
public:
    template<class Machine, class Context>
    region(Machine& mach, Context& ctx):
        states_(uniform_construct, mach, ctx)
    {
    }

    region(const region&) = delete;
    region(region&&) = delete;
    region& operator=(const region&) = delete;
    region& operator=(region&&) = delete;
    ~region() = default;

    template<const auto& StatePath, class ParentContext>
    const auto& context_or(ParentContext& parent_ctx) const
    {
        return static_context_or<StatePath>(*this, parent_ctx);
    }

    template<const auto& StatePath, class ParentContext>
    auto& context_or(ParentContext& parent_ctx)
    {
        return static_context_or<StatePath>(*this, parent_ctx);
    }

    template<const auto& RegionPath, auto StateId>
    [[nodiscard]] bool active_state() const
    {
        if constexpr(RegionPath.empty())
        {
            return active_state<StateId>();
        }
        else
        {
            static constexpr auto psubmach_conf = path_raw_head(RegionPath);
            static constexpr auto region_path_tail = path_tail(RegionPath);
            const auto& submach = state_from_id<psubmach_conf>();
            return submach.template active_state<region_path_tail, *StateId>();
        }
    }

    template<auto StateId>
    [[nodiscard]] bool active_state() const
    {
        if constexpr(is_filter_v<std::decay_t<decltype(*StateId)>>)
        {
            return does_active_state_id_match_filter<StateId>();
        }
        else
        {
            return is_active_state_type<StateId>();
        }
    }

    template<class Machine, class Context, class Event>
    void start(Machine& mach, Context& ctx, const Event& event)
    {
        if(is_active_state_type<&state_confs::stopped>())
        {
            process_event_in_transition
            <
                &state_confs::stopped,
                pinitial_state_conf,
                &null
            >(mach, ctx, event);
        }
    }

    template<class Machine, class Context, class Event>
    void stop(Machine& mach, Context& ctx, const Event& event)
    {
        if(!is_active_state_type<&state_confs::stopped>())
        {
            with_active_state_id<state_id_constant_list, stop_2>
            (
                *this,
                mach,
                ctx,
                event
            );
        }
    }

    template<bool Dry, class Machine, class Context, class Event>
    void process_event(Machine& mach, Context& ctx, const Event& event)
    {
        process_event_2<Dry>(*this, mach, ctx, event);
    }

    template<bool Dry, class Machine, class Context, class Event>
    void process_event(Machine& mach, Context& ctx, const Event& event, bool& processed)
    {
        process_event_2<Dry>(*this, mach, ctx, event, processed);
    }

    template<bool Dry, class Machine, class Context, class Event>
    void process_event(Machine& mach, Context& ctx, const Event& event) const
    {
        process_event_2<Dry>(*this, mach, ctx, event);
    }

    template<bool Dry, class Machine, class Context, class Event>
    void process_event(Machine& mach, Context& ctx, const Event& event, bool& processed) const
    {
        process_event_2<Dry>(*this, mach, ctx, event, processed);
    }

private:
    static constexpr const auto& transition_table = tuple_get<Index>(opts(ParentSm::conf).transition_tables);
    static constexpr auto transition_tuple = detail::rows(transition_table);

    using transition_table_digest_type =
        detail::transition_table_digest<transition_tuple>
    ;

    using state_id_constant_list = typename transition_table_digest_type::state_id_constant_list;

    template<class... StateIdConstants>
    using state_id_constant_pack_to_state_tuple_t = tuple
    <
        state_traits::state_id_to_state_t<StateIdConstants::value, region>...
    >;

    using state_tuple_type = tlu::apply_t
    <
        state_id_constant_list,
        state_id_constant_pack_to_state_tuple_t
    >;

    static constexpr auto pinitial_state_conf = detail::tlu::front_t<state_id_constant_list>::value;

    template<bool Dry, class Self, class Machine, class Context, class Event, class... MaybeBool>
    static void process_event_2
    (
        Self& self,
        Machine& mach,
        Context& ctx,
        const Event& event,
        [[maybe_unused]] MaybeBool&... processed
    )
    {
        //List the transitions whose event type filter matches Event
        using candidate_transition_index_constant_list = transition_table_filters::by_event_t
        <
            transition_tuple,
            Event
        >;

        //List the state types that require us to call their on_event()
        using candidate_state_type_list =
            state_type_list_filters::by_required_on_event_t
            <
                state_tuple_type,
                region,
                Event
            >
        ;

        constexpr auto must_try_processing_event_in_transitions = !tlu::empty_v<candidate_transition_index_constant_list>;
        constexpr auto must_try_processing_event_in_active_state = !tlu::empty_v<candidate_state_type_list>;

        if constexpr(must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
        {
            if(try_processing_event_in_transitions<candidate_transition_index_constant_list, Dry>(self, mach, ctx, event))
            {
                maybe_bool_util::set_to_true(processed...);
            }
            else
            {
                try_processing_event_in_active_state<candidate_state_type_list, Dry>(self, mach, ctx, event, processed...);
            }
        }
        else if constexpr(!must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
        {
            try_processing_event_in_active_state<candidate_state_type_list, Dry>(self, mach, ctx, event, processed...);
        }
        else if constexpr(must_try_processing_event_in_transitions && !must_try_processing_event_in_active_state)
        {
            try_processing_event_in_transitions<candidate_transition_index_constant_list, Dry>(self, mach, ctx, event, processed...);
        }
    }

    struct stop_2
    {
        template<class ActiveStateIdConstant, class Machine, class Context, class Event>
        static void call(region& self, Machine& mach, Context& ctx, const Event& event)
        {
            self.process_event_in_transition
            <
                ActiveStateIdConstant::value,
                &state_confs::stopped,
                &null
            >(mach, ctx, event);
        }
    };

    template<class TransitionIndexConstantList, bool Dry = false, class Self, class Machine, class Context, class Event, class... ExtraArgs>
    static bool try_processing_event_in_transitions(Self& self, Machine& mach, Context& ctx, const Event& event, ExtraArgs&... extra_args)
    {
        return tlu::for_each_or
        <
            TransitionIndexConstantList,
            try_processing_event_in_transition<Dry>
        >(self, mach, ctx, event, extra_args...);
    }

    //Check active state and guard
    template<bool Dry>
    struct try_processing_event_in_transition
    {
        template<class TransitionIndexConstant, class Self, class Machine, class Context, class Event, class... ExtraArgs>
        static bool call(Self& self, Machine& mach, Context& ctx, const Event& event, ExtraArgs&... extra_args)
        {
            static constexpr const auto& trans = tuple_get<TransitionIndexConstant::value>(transition_tuple);
            static constexpr auto source_state_conf_filter = trans.source_state_conf_filter;
            static constexpr auto action = trans.action;
            static constexpr auto guard = trans.guard;

            if constexpr(is_filter_v<std::decay_t<decltype(source_state_conf_filter)>>)
            {
                //List of state confs that match with the source state filter
                using matching_state_conf_constant_list = state_type_list_filters::by_filter_t
                <
                    state_id_constant_list,
                    &source_state_conf_filter
                >;

                static_assert(!tlu::empty_v<matching_state_conf_constant_list>);

                return tlu::for_each_or
                <
                    matching_state_conf_constant_list,
                    try_processing_event_in_transition_2
                    <
                        Dry,
                        trans.target_state_conf,
                        action,
                        guard
                    >
                >(self, mach, ctx, event, extra_args...);
            }
            else
            {
                return try_processing_event_in_transition_2
                <
                    Dry,
                    trans.target_state_conf,
                    action,
                    guard
                >::template call<constant_t<trans.source_state_conf_filter>>
                (
                    self,
                    mach,
                    ctx,
                    event,
                    extra_args...
                );
            }
        }
    };

    template<bool Dry, auto TargetStateId, const auto& Action, const auto& Guard>
    struct try_processing_event_in_transition_2
    {
        template
        <
            class SourceStateIdConstant,
            class Self,
            class Machine,
            class Context,
            class Event,
            class... MaybeBool
        >
        static bool call
        (
            Self& self,
            Machine& mach,
            Context& ctx,
            const Event& event,
            MaybeBool&... processed
        )
        {
            //Make sure the transition source state is the active state
            if(!self.template is_active_state_type<SourceStateIdConstant::value>())
            {
                return false;
            }

            //Check guard
            if constexpr(!std::is_same_v<decltype(Guard), const null_t&>)
            {
                if(!detail::call_action_or_guard(Guard, mach, ctx, event))
                {
                    return false;
                }
            }

            if constexpr(!Dry)
            {
                self.template process_event_in_transition
                <
                    SourceStateIdConstant::value,
                    TargetStateId,
                    &Action
                >(mach, ctx, event);
            }

            maybe_bool_util::set_to_true(processed...);

            return true;
        }
    };

    template
    <
        auto SourceStateId,
        auto TargetStateId,
        auto ActionPtr,
        class Machine,
        class Context,
        class Event
    >
    void process_event_in_transition
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        using machine_option_set_type = typename Machine::option_set_type;

        constexpr const auto& path = path_of_v<region>;

        constexpr auto is_internal_transition = std::is_same_v
        <
            std::decay_t<decltype(TargetStateId)>,
            null_t
        >;

        if constexpr(!is_internal_transition)
        {
            if constexpr(!std::is_same_v<typename machine_option_set_type::pre_state_transition_action_type, null_t>)
            {
                opts(Machine::conf).pre_state_transition_action
                (
                    ctx,
                    cref_constant<path>,
                    cref_constant<*SourceStateId>,
                    event,
                    cref_constant<*TargetStateId>
                );
            }

            if constexpr(!same_ref(*SourceStateId, state_confs::stopped))
            {
                auto& stt = state_from_id<SourceStateId>();
                stt.call_exit_action
                (
                    mach,
                    ctx,
                    event
                );
            }

            active_state_index_ = region_detail::find_state_from_id_v
            <
                state_tuple_type,
                TargetStateId
            >;
        }

        if constexpr(!std::is_same_v<decltype(ActionPtr), const null_t*>)
        {
            detail::call_action_or_guard
            (
                *ActionPtr,
                mach,
                ctx,
                event
            );
        }

        if constexpr(!is_internal_transition)
        {
            if constexpr(!same_ref(*TargetStateId, state_confs::stopped))
            {
                auto& stt = state_from_id<TargetStateId>();
                stt.call_entry_action
                (
                    mach,
                    ctx,
                    event
                );
            }

            if constexpr(!std::is_same_v<typename machine_option_set_type::post_state_transition_action_type, null_t>)
            {
                opts(Machine::conf).post_state_transition_action
                (
                    ctx,
                    cref_constant<path>,
                    cref_constant<*SourceStateId>,
                    event,
                    cref_constant<*TargetStateId>
                );
            }

            //Anonymous transition
            if constexpr(transition_table_digest_type::has_null_events)
            {
                using candidate_transition_constant_list = transition_table_filters::by_null_event_t
                <
                    transition_tuple
                >;

                try_processing_event_in_transitions<candidate_transition_constant_list>(*this, mach, ctx, null);
            }
        }
    }

    /*
    Call active_state.on_event(event)
    */
    template<class StateTypeList, bool Dry, class Self, class Machine, class Context, class Event, class... ExtraArgs>
    static void try_processing_event_in_active_state
    (
        Self& self,
        Machine& mach,
        Context& ctx,
        const Event& event,
        ExtraArgs&... extra_args
    )
    {
        tlu::for_each_or
        <
            StateTypeList,
            try_processing_event_in_active_state_2<Dry>
        >(self, mach, ctx, event, extra_args...);
    }

    template<bool Dry>
    struct try_processing_event_in_active_state_2
    {
        template<class State, class Self, class Machine, class Context, class Event, class... ExtraArgs>
        static bool call
        (
            Self& self,
            Machine& mach,
            Context& ctx,
            const Event& event,
            ExtraArgs&... extra_args
        )
        {
            if(!self.template is_active_state_type<State>())
            {
                return false;
            }

            if constexpr(!Dry)
            {
                self.template state<State>().call_internal_action
                (
                    mach,
                    ctx,
                    event,
                    extra_args...
                );
            }

            return true;
        }
    };

    template<class State>
    [[nodiscard]] bool is_active_state_type() const
    {
        constexpr auto given_state_index = region_detail::find_state_v
        <
            state_tuple_type,
            State
        >;
        return given_state_index == active_state_index_;
    }

    template<auto StateId>
    [[nodiscard]] bool is_active_state_type() const
    {
        constexpr auto given_state_index = region_detail::find_state_from_id_v
        <
            state_tuple_type,
            StateId
        >;
        return given_state_index == active_state_index_;
    }

    template<auto FilterPtr>
    [[nodiscard]] bool does_active_state_id_match_filter() const
    {
        auto matches = false;
        with_active_state_id
        <
            tlu::push_back_t<state_id_constant_list, constant_t<&state_confs::stopped>>,
            does_active_state_id_match_filter_2<FilterPtr>
        >(matches);
        return matches;
    }

    template<auto FilterPtr>
    struct does_active_state_id_match_filter_2
    {
        template<class ActiveStateIdConstant>
        static void call([[maybe_unused]] bool& matches)
        {
            if constexpr(matches_filter_ptr(ActiveStateIdConstant::value, FilterPtr))
            {
                matches = true;
            }
        }
    };

    template<class StateIdConstantList, class F, class... Args>
    void with_active_state_id(Args&&... args) const
    {
        tlu::for_each_or
        <
            StateIdConstantList,
            with_active_state_id_2<F>
        >(*this, std::forward<Args>(args)...);
    }

    template<class F>
    struct with_active_state_id_2
    {
        template<class StateIdConstant, class... Args>
        static bool call(const region& self, Args&&... args)
        {
            if(self.is_active_state_type<StateIdConstant::value>())
            {
                F::template call<StateIdConstant>(std::forward<Args>(args)...);
                return true;
            }
            return false;
        }
    };

    template<class State>
    auto& state()
    {
        return static_state<State>(*this);
    }

    template<auto StateId>
    auto& state_from_id()
    {
        return static_state_from_id<StateId>(*this);
    }

    template<auto StateId>
    const auto& state_from_id() const
    {
        return static_state_from_id<StateId>(*this);
    }

    //Note: We use static to factorize const and non-const Region
    template<class State, class Region>
    static auto& static_state(Region& self)
    {
        static constexpr auto state_index =
            region_detail::find_state_v<state_tuple_type, State>
        ;
        return tuple_get<state_index>(self.states_);
    }

    //Note: We use static to factorize const and non-const Region
    template<auto StateId, class Region>
    static auto& static_state_from_id(Region& self)
    {
        static constexpr auto state_index =
            region_detail::find_state_from_id_v<state_tuple_type, StateId>
        ;
        return tuple_get<state_index>(self.states_);
    }

    //Note: We use static to factorize const and non-const Region
    template<const auto& StatePath, class Region, class ParentContext>
    static auto& static_context_or(Region& self, ParentContext& parent_ctx)
    {
        if constexpr(StatePath.size() == 1)
        {
            return static_state_from_id<path_raw_head(StatePath)>(self).context_or(parent_ctx);
        }
        else
        {
            static constexpr auto psubmach_conf = path_raw_head(StatePath);
            static constexpr auto state_path_tail = path_tail(StatePath);
            return static_state_from_id<psubmach_conf>(self).template context_or<state_path_tail>(parent_ctx);
        }
    }

    state_tuple_type states_;
    int active_state_index_ = region_detail::stopped_state_index;
};

} //namespace

#endif
