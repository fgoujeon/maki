//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_COMPOSITE_STATE_HPP
#define AWESM_COMPOSITE_STATE_HPP

#include "detail/region_tuple.hpp"
#include "detail/sm_object_holder.hpp"
#include "detail/sm_path.hpp"

namespace awesm
{

template<class Definition>
class composite_state
{
    public:
        template<class Sm, class Context>
        composite_state(Sm& mach, Context& ctx):
            def_(mach, ctx),
            region_tuple_(mach, ctx)
        {
        }

        template<class State, int RegionIndex = 0>
        [[nodiscard]] bool is_active_state() const
        {
            return region_tuple_.template is_active_state<State, RegionIndex>();
        }

        [[nodiscard]] bool is_running() const
        {
            return !is_active_state<detail::null_state>();
        }

        template<class RegionPath, class Sm, class Event>
        void on_entry(Sm& mach, const Event& event)
        {
            using sm_path_t = detail::sm_path<RegionPath, composite_state>;
            def_.get_object().on_entry(event);
            region_tuple_.template start<sm_path_t>(mach, event);
        }

        template<class RegionPath, class Sm, class Event>
        void on_event(Sm& mach, const Event& event)
        {
            using sm_path_t = detail::sm_path<RegionPath, composite_state>;
            region_tuple_.template process_event<sm_path_t>(mach, event);
            def_.get_object().on_event(event);
        }

        template<class RegionPath, class Sm, class Event>
        void on_exit(Sm& mach, const Event& event)
        {
            using sm_path_t = detail::sm_path<RegionPath, composite_state>;
            region_tuple_.template stop<sm_path_t>(mach, event);
            def_.get_object().on_exit(event);
        }

    private:
        detail::sm_object_holder<Definition> def_;
        detail::region_tuple<typename Definition::transition_tables> region_tuple_;
};

} //namespace

#endif
