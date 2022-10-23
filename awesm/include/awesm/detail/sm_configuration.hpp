//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_CONFIGURATION_HPP
#define AWESM_DETAIL_SM_CONFIGURATION_HPP

#include "sm_object_holder.hpp"
#include "../sm_options.hpp"

namespace awesm::detail
{

template<class... Options>
class completed_sm_configuration: private Options...
{
    public:
        template<class Sm, class Context>
        completed_sm_configuration(Sm& mach, Context& ctx):
            Options(mach, ctx)...
        {
        }

        static constexpr bool has_in_state_internal_transitions()
        {
            return call
            (
                sm_options::detail::tags::in_state_internal_transitions{},
                0
            );
        }

        static constexpr bool has_run_to_completion()
        {
            return call
            (
                sm_options::detail::tags::run_to_completion{},
                0
            );
        }

        template<int RegionIndex, class SourceState, class Event, class TargetState>
        void after_state_transition(const Event& event)
        {
            call
            (
                sm_options::detail::tags::after_state_transition{},
                std::integral_constant<int, RegionIndex>{},
                static_cast<SourceState*>(nullptr),
                &event,
                static_cast<TargetState*>(nullptr),
                0
            );
        }

        template<int RegionIndex, class SourceState, class Event, class TargetState>
        void before_entry(const Event& event)
        {
            call
            (
                sm_options::detail::tags::before_entry{},
                std::integral_constant<int, RegionIndex>{},
                static_cast<SourceState*>(nullptr),
                &event,
                static_cast<TargetState*>(nullptr),
                0
            );
        }

        template<class Event>
        auto on_event(const Event* pevent) ->
            decltype(call(sm_options::detail::tags::on_event{}, *pevent, 0))
        {
            call(sm_options::detail::tags::on_event{}, *pevent, 0);
        }

        void on_event(const void* /*pevent*/)
        {
        }

        void on_exception(const std::exception_ptr& eptr)
        {
            call(sm_options::detail::tags::on_exception{}, eptr, 0);
        }

    private:
        using Options::call...;
};

template<class... Options>
using sm_configuration = completed_sm_configuration
<
    sm_options::detail::defaults::after_state_transition,
    sm_options::detail::defaults::before_entry,
    sm_options::detail::defaults::in_state_internal_transitions,
    sm_options::detail::defaults::on_event,
    sm_options::detail::defaults::on_exception,
    sm_options::detail::defaults::run_to_completion,
    Options...
>;

} //namespace

#endif
