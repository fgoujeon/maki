//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_DEFAULT_PRE_TRANSITION_EVENT_HANDLER_HPP
#define AWESM_DETAIL_DEFAULT_PRE_TRANSITION_EVENT_HANDLER_HPP

namespace awesm::detail
{

struct default_pre_transition_event_handler
{
    template<class Sm, class Context>
    default_pre_transition_event_handler(Sm& /*machine*/, Context& /*ctx*/)
    {
    }

    /*
    Called whenever an event is being processed, after recursive call
    protection and just before performing internal and state
    transitions.
    It is useful for handling event independently of the active state by
    taking advantage of the run-to-completion mechanism.
    */
    //void on_event(const Event&);
};

} //namespace

#endif
