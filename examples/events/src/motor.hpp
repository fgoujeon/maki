//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

//! [all]
#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <variant>
#include <functional>

/*
The motor driver

Since the motor takes time to start and stop, the interface is asynchronous.
It sends events to notify the client when the asynchronous commands complete.
*/
class motor
{
public:
    //Events
    struct start_event{};
    struct stop_event{};

    using event_type = std::variant<start_event, stop_event>;
    using event_callback_type = std::function<void(const event_type&)>;

    motor(const event_callback_type& cb):
        cb_(cb)
    {
        //[Implementation detail...]
    }

    //Asynchronously start the motor and send a start_event when it's done
    void async_start()
    {
        //[Implementation detail...]
    }

    //Asynchronously stop the motor and send a stop_event when it's done
    void async_stop()
    {
        //[Implementation detail...]
    }

private:
    //[Implementation detail...]

    event_callback_type cb_;
};

#endif
//! [all]
