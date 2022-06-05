//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SUBSM_HPP
#define AWESM_SUBSM_HPP

#include "subsm_configuration.hpp"
#include "detail/region.hpp"
#include <type_traits>

namespace awesm
{

namespace detail
{
    template<class SubsmConfiguration, class RegionConfList>
    struct subsm_configuration_to_region_tuple_helper;

    template<class SubsmConfiguration, template<class...> class RegionConfList, class... RegionConfs>
    struct subsm_configuration_to_region_tuple_helper<SubsmConfiguration, RegionConfList<RegionConfs...>>
    {
        struct region_private_configuration
        {
            using exception_handler = typename SubsmConfiguration::exception_handler;
            using state_transition_hook_set = typename SubsmConfiguration::state_transition_hook_set;
            static constexpr auto enable_in_state_internal_transitions = SubsmConfiguration::enable_in_state_internal_transitions;
        };

        using type = sm_object_holder_tuple
        <
            region<RegionConfs, region_private_configuration>...
        >;
    };

    template<class SubsmConfiguration>
    using subsm_configuration_to_region_tuple =
        typename subsm_configuration_to_region_tuple_helper
        <
            SubsmConfiguration,
            typename SubsmConfiguration::region_configurations
        >::type
    ;
}

template<class Configuration>
class subsm
{
    public:
        static_assert
        (
            std::is_base_of_v<subsm_configuration, Configuration>,
            "Given configuration type must inherit from awesm::subsm_configuration."
        );

        template<class Sm, class Context>
        explicit subsm(Sm& top_level_sm, Context& context):
            regions_{top_level_sm, context}
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

        void reset()
        {
            regions_.for_each
            (
                [&](auto& reg)
                {
                    reg.reset();
                }
            );
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
        using region_tuple_t = detail::subsm_configuration_to_region_tuple
        <
            Configuration
        >;

        region_tuple_t regions_;
};

} //namespace

#endif
