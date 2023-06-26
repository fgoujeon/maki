//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

//! [all]
#ifndef USER_INTERFACE_HPP
#define USER_INTERFACE_HPP

#include <variant>
#include <functional>

/*
The user interface monitor

This monitor sends events to notify user actions.
*/
class user_interface
{
public:
    //Events
    struct start_request{};
    struct stop_request{};

    using event_type = std::variant<start_request, stop_request>;
    using event_callback_type = std::function<void(const event_type&)>;

    user_interface(const event_callback_type& cb):
        cb_(cb)
    {
        //[Implementation detail...]
    }

private:
    //[Implementation detail...]

    event_callback_type cb_;
};

#endif
//! [all]
