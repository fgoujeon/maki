//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_EVENT_HPP
#define FGFSM_EVENT_HPP

#include "none.hpp"
#include <any>

namespace fgfsm
{

class event
{
    public:
        template<class Event>
        event(const Event& event):
            any_(event)
        {
        }

        template<class Event>
        const Event* get() const
        {
            return std::any_cast<Event>(&any_);
        }

        template<class Event>
        bool is() const
        {
            return get<Event>() != nullptr;
        }

    private:
        std::any any_;
};

} //namespace

#endif
