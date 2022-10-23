//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_OPTIONS_HPP
#define AWESM_SM_OPTIONS_HPP

#include "sm_options/in_state_internal_transitions.hpp"

namespace awesm::sm_options
{

struct after_state_transition
{
    //Dummy
    template<class Sm, class Context>
    after_state_transition(Sm& /*sm*/, Context& /*ctx*/)
    {
    }

    //Dummy
    void call()
    {
    }
};

struct before_entry
{
    //Dummy
    template<class Sm, class Context>
    before_entry(Sm& /*sm*/, Context& /*ctx*/)
    {
    }

    //Dummy
    void call()
    {
    }
};

struct before_state_transition
{
    //Dummy
    template<class Sm, class Context>
    before_state_transition(Sm& /*sm*/, Context& /*ctx*/)
    {
    }

    //Dummy
    void call()
    {
    }
};

struct disable_run_to_completion
{
    //Dummy
    template<class Sm, class Context>
    disable_run_to_completion(Sm& /*sm*/, Context& /*ctx*/)
    {
    }

    //Dummy
    void call()
    {
    }
};

struct on_event
{
    //Dummy
    template<class Sm, class Context>
    on_event(Sm& /*sm*/, Context& /*ctx*/)
    {
    }

    //Dummy
    void call()
    {
    }
};

struct on_exception
{
    //Dummy
    template<class Sm, class Context>
    on_exception(Sm& /*sm*/, Context& /*ctx*/)
    {
    }

    //Dummy
    void call()
    {
    }
};

} //namespace

#endif
