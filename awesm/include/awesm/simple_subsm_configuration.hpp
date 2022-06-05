//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SIMPLE_SUBSM_CONFIGURATION_HPP
#define AWESM_SIMPLE_SUBSM_CONFIGURATION_HPP

#include "sm_ref.hpp"
#include <exception>

namespace awesm
{

struct simple_subsm_configuration
{
    class exception_handler
    {
        public:
            template<class Sm, class Context>
            exception_handler(Sm& m, Context& /*ctx*/):
                sm_(m)
            {
            }

            void on_exception(const std::exception_ptr& e)
            {
                sm_.process_event(e);
            }

        private:
            sm_ref<std::exception_ptr> sm_;
    };

    struct state_transition_hook_set
    {
        template<class Sm, class Context>
        state_transition_hook_set(Sm& /*machine*/, Context& /*ctx*/)
        {
        }

        template<class SourceState, class Event, class TargetState>
        void before_transition(const Event& /*event*/)
        {
        }

        template<class SourceState, class Event, class TargetState>
        void after_transition(const Event& /*event*/)
        {
        }
    };

    static constexpr auto enable_in_state_internal_transitions = true;
};

} //namespace

#endif
