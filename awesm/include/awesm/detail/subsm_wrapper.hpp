//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_COMPOSITE_STATE_WRAPPER_HPP
#define AWESM_DETAIL_COMPOSITE_STATE_WRAPPER_HPP

#include "../state_conf.hpp"
#include "call_member.hpp"
#include "region_tuple.hpp"
#include "sm_object_holder.hpp"
#include "sm_path.hpp"
#include "sm_traits.hpp"

namespace awesm::detail
{

template<class RegionPath, class WrappedState>
class subsm_wrapper
{
    public:
        using sm_type = region_path_to_sm_t<RegionPath>;

        using conf = state_conf
        <
            state_options::on_entry_any,
            state_options::on_event_any,
            state_options::on_exit_any,
            state_options::get_pretty_name
        >;

        template<class Context>
        subsm_wrapper(sm_type& mach, Context& ctx):
            state_holder_(mach, ctx),
            region_tuple_(mach)
        {
        }

        template<class StateRelativeRegionPath, class State>
        [[nodiscard]] bool is_active_state() const
        {
            return region_tuple_.template is_active_state<StateRelativeRegionPath, State>();
        }

        template<class State>
        [[nodiscard]] bool is_active_state() const
        {
            return region_tuple_.template is_active_state<State>();
        }

        [[nodiscard]] bool is_running() const
        {
            return !is_active_state<states::stopped>();
        }

        template<class Event>
        void on_entry(const Event& event)
        {
            call_on_entry(state_holder_.get(), event);
            region_tuple_.start(event);
        }

        template<class Event>
        void on_event(const Event& event)
        {
            region_tuple_.process_event(event);
            call_on_event(state_holder_.get(), event);
        }

        template<class Event>
        void on_exit(const Event& event)
        {
            region_tuple_.stop(event);
            call_on_exit(state_holder_.get(), event);
        }

        static decltype(auto) get_pretty_name()
        {
            return awesm::get_pretty_name<WrappedState>();
        }

    private:
        using transition_table_fn_list_type = sm_traits::transition_table_fn_list_t<WrappedState>;
        using sm_path_type = detail::sm_path<RegionPath, WrappedState>;

        detail::sm_object_holder<WrappedState> state_holder_;
        detail::region_tuple<sm_path_type, transition_table_fn_list_type> region_tuple_;
};

} //namespace

#endif
