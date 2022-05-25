//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_CONFIGURATION_HPP
#define AWESM_SM_CONFIGURATION_HPP

#include <exception>

namespace awesm
{

struct sm_configuration
{
    struct pre_transition_event_handler
    {
        template<class Context, class Sm>
        pre_transition_event_handler(Context& /*ctx*/, Sm& /*machine*/)
        {
            /*
            Called whenever an event is being processed, after recursive call
            protection and just before performing internal and state
            transitions.
            It is useful for handling event independently of the active state by
            taking advantage of the run-to-completion mechanism.
            */
            //void on_event(const Event&);
        }
    };

    template<class Sm>
    struct internal_transition_policy
    {
        template<class Context>
        internal_transition_policy(Context& /*ctx*/, Sm& machine):
            machine(machine)
        {
        }

        template<class Helper>
        void do_transition(Helper& helper)
        {
            try
            {
                helper.invoke_state_on_event();
            }
            catch(...)
            {
                machine.process_event(std::current_exception());
            }
        }

        Sm& machine;
    };

    static constexpr auto enable_run_to_completion = true;
    static constexpr auto enable_in_state_internal_transitions = true;
};

} //namespace

#endif
