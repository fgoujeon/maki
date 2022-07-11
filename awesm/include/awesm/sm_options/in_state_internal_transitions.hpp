//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_OPTIONS_IN_STATE_INTERNAL_TRANSITIONS_HPP
#define AWESM_SM_OPTIONS_IN_STATE_INTERNAL_TRANSITIONS_HPP

namespace awesm::sm_options
{

namespace detail::tags
{
    struct in_state_internal_transitions{};
}

namespace detail::defaults
{
    struct in_state_internal_transitions
    {
        template<class Sm, class Context>
        in_state_internal_transitions(Sm& /*m*/, Context& /*ctx*/)
        {
        }

        static constexpr bool call(tags::in_state_internal_transitions /*tag*/, long /*priority*/)
        {
            return true;
        }
    };
}

template<bool Enabled>
class in_state_internal_transitions
{
    public:
        template<class Sm, class Context>
        in_state_internal_transitions(Sm& /*sm*/, Context& /*ctx*/)
        {
        }

        static constexpr bool call(detail::tags::in_state_internal_transitions /*tag*/, int /*priority*/)
        {
            return Enabled;
        }
};

} //namespace

#endif
