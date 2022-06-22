//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SUBSM_HPP
#define AWESM_SUBSM_HPP

#include "detail/region.hpp"
#include "detail/sm_configuration.hpp"
#include <type_traits>

namespace awesm
{

namespace detail
{
    template<class RegionConfigurationList, class... Options>
    struct region_tuple_helper;

    template<template<class...> class RegionConfigurationList, class... RegionConfs, class... Options>
    struct region_tuple_helper<RegionConfigurationList<RegionConfs...>, Options...>
    {
        using type = sm_object_holder_tuple
        <
            region<RegionConfs, Options...>...
        >;
    };

    template<class RegionConfigurationList, class... Options>
    using region_tuple =
        typename region_tuple_helper
        <
            RegionConfigurationList,
            Options...
        >::type
    ;
}

template<class MainConfiguration, class... Options>
class subsm
{
    public:
        template<class Sm, class Context>
        explicit subsm(Sm& top_level_sm, Context& context):
            regions_{top_level_sm, context},
            conf_(top_level_sm, context)
        {
        }

        subsm(const subsm&) = delete;
        subsm(subsm&&) = delete;
        subsm& operator=(const subsm&) = delete;
        subsm& operator=(subsm&&) = delete;
        ~subsm() = default;

        template<class State, int RegionIndex = 0>
        [[nodiscard]] bool is_active_state() const
        {
            return regions_.template get<RegionIndex>().template is_active_state<State>();
        }

        template<class Event>
        void start(const Event& event)
        {
            regions_.for_each
            (
                [&](auto& reg)
                {
                    reg.start(event);
                }
            );
        }

        void start()
        {
            regions_.for_each
            (
                [&](auto& reg)
                {
                    reg.start();
                }
            );
        }

        template<class Event>
        void stop(const Event& event)
        {
            regions_.for_each
            (
                [&](auto& reg)
                {
                    reg.stop(event);
                }
            );
        }

        void stop()
        {
            regions_.for_each
            (
                [&](auto& reg)
                {
                    reg.stop();
                }
            );
        }

        template<class Event>
        void process_event(const Event& event)
        {
            regions_.for_each
            (
                [&](auto& reg)
                {
                    reg.process_event(event);
                }
            );
        }

    private:
        using region_tuple_t = detail::region_tuple
        <
            typename MainConfiguration::region_configurations,
            Options...
        >;

        using configuration_t = detail::sm_configuration
        <
            sm_options::detail::defaults::after_state_transition,
            sm_options::detail::defaults::before_state_transition,
            sm_options::detail::defaults::in_state_internal_transitions,
            sm_options::detail::defaults::on_event,
            sm_options::detail::defaults::on_exception,
            sm_options::detail::defaults::run_to_completion,
            Options...
        >;

        region_tuple_t regions_;
        configuration_t conf_;
};

} //namespace

#endif
