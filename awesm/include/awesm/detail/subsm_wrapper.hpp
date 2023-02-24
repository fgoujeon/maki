//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SUBSM_WRAPPER_HPP
#define AWESM_DETAIL_SUBSM_WRAPPER_HPP

#include "../state_conf.hpp"
#include "call_member.hpp"
#include "region_tuple.hpp"
#include "sm_object_holder.hpp"
#include "sm_conf_traits.hpp"
#include "region_path_of.hpp"

namespace awesm::detail
{

template<class Subsm, class Region>
struct region_path_of<subsm_wrapper<Subsm, Region>>
{
    using type = region_path_of_t<Region>;
};

template<class Subsm, class Region>
struct root_sm_of<subsm_wrapper<Subsm, Region>>
{
    using type = root_sm_of_t<Region>;
};

template<class Subsm, class Region>
class subsm_wrapper
{
    public:
        using root_sm_type = root_sm_of_t<subsm_wrapper>;
        using subsm_conf_type = typename Subsm::conf;
        using parent_sm_context_type = typename Region::parent_sm_type::context_type;

        /*
        Context type is either (in this order of priority):
        - the one specified in the subsm_opts::context option, if any;
        - the context type of the parent SM (not necessarily the root SM).
        */
        using context_type = sm_conf_traits::context_t
        <
            subsm_conf_type,
            parent_sm_context_type&
        >;

        using conf = state_conf
        <
            state_opts::on_entry_any,
            state_opts::on_event_any,
            state_opts::on_exit_any,
            state_opts::get_pretty_name
        >;

        subsm_wrapper(root_sm_type& root_sm, parent_sm_context_type& parent_ctx):
            root_sm_(root_sm),
            context_(parent_ctx),
            subsm_holder_(root_sm, context_),
            region_tuple_(*this, context_)
        {
        }

        root_sm_type& get_root_sm()
        {
            return root_sm_;
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
            call_on_entry(subsm_holder_.get(), root_sm_, event);
            region_tuple_.start(event);
        }

        template<class Event>
        void on_event(const Event& event)
        {
            call_on_event(subsm_holder_.get(), event);
            region_tuple_.process_event(event);
        }

        template<class Event>
        void on_exit(const Event& event)
        {
            region_tuple_.stop(event);
            call_on_exit(subsm_holder_.get(), root_sm_, event);
        }

        static decltype(auto) get_pretty_name()
        {
            return awesm::get_pretty_name<Subsm>();
        }

    private:
        using transition_table_fn_list_type = sm_conf_traits::transition_table_fn_list_t<subsm_conf_type>;

        root_sm_type& root_sm_;
        context_type context_;
        detail::sm_object_holder<Subsm> subsm_holder_;
        detail::region_tuple<subsm_wrapper, transition_table_fn_list_type> region_tuple_;
};

} //namespace

#endif
