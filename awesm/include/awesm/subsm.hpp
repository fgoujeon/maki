//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SUBSM_HPP
#define AWESM_SUBSM_HPP

#include "detail/region.hpp"
#include "region_configuration_list.hpp"
#include <type_traits>

namespace awesm
{

namespace detail
{
    template<class RegionConfigurationList, class... Options>
    struct region_tuple_helper;

    template<class... RegionConfs, class... Options>
    struct region_tuple_helper<region_configuration_list<RegionConfs...>, Options...>
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

template<class RegionConfListHolder, class... Options>
class subsm
{
    public:
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

        template<class SmConfiguration, class Event = none>
        void start(SmConfiguration& sm_conf, const Event& event = {})
        {
            regions_.for_each
            (
                [&](auto& reg)
                {
                    reg.start(sm_conf, event);
                }
            );
        }

        template<class SmConfiguration, class Event = none>
        void stop(SmConfiguration& sm_conf, const Event& event = {})
        {
            regions_.for_each
            (
                [&](auto& reg)
                {
                    reg.stop(sm_conf, event);
                }
            );
        }

        template<class SmConfiguration, class Event>
        void process_event(SmConfiguration& sm_conf, const Event& event)
        {
            regions_.for_each
            (
                [&](auto& reg)
                {
                    reg.process_event(sm_conf, event);
                }
            );
        }

    private:
        using region_tuple_t = detail::region_tuple
        <
            typename RegionConfListHolder::type,
            Options...
        >;

        region_tuple_t regions_;
};

} //namespace

#endif
