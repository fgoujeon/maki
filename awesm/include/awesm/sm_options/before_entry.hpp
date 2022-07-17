//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_OPTIONS_BEFORE_ENTRY_HPP
#define AWESM_SM_OPTIONS_BEFORE_ENTRY_HPP

#include "../detail/sm_object_holder.hpp"

namespace awesm::sm_options
{

namespace detail::tags
{
    struct before_entry{};
}

namespace detail::defaults
{
    struct before_entry
    {
        template<class Sm, class Context>
        before_entry(Sm& /*m*/, Context& /*ctx*/)
        {
        }

        void call
        (
            tags::before_entry /*action_tag*/,
            const void* /*pregion*/,
            void* /*source_state_tag*/,
            const void* /*pevent*/,
            void* /*target_state_tag*/,
            long /*priority*/
        )
        {
        }
    };
}

template<class T>
class before_entry
{
    public:
        template<class Sm, class Context>
        before_entry(Sm& m, Context& ctx):
            impl_(m, ctx)
        {
        }

        template<class Region, class SourceState, class Event, class TargetState>
        void call
        (
            detail::tags::before_entry /*action_tag*/,
            const Region* pregion,
            SourceState* /*source_state_tag*/,
            const Event* pevent,
            TargetState* /*target_state_tag*/,
            int /*priority*/
        )
        {
            impl_.object.template before_entry<Region, SourceState, Event, TargetState>
            (
                *pregion,
                *pevent
            );
        }

    private:
        awesm::detail::sm_object_holder<T> impl_;
};

} //namespace

#endif
