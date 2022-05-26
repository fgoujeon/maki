//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_GUARD_OPERATORS_HPP
#define AWESM_GUARD_OPERATORS_HPP

#include "detail/sm_object_holder_tuple.hpp"
#include "detail/sm_object_holder.hpp"
#include "detail/call_member.hpp"

namespace awesm
{

template<class... Guards>
class and_
{
    public:
        template<class Context, class Sm>
        and_(Context& context, Sm& machine):
            guards_(context, machine)
        {
        }

        template<class Event>
        bool check(const Event& event)
        {
            return
            (
                detail::call_check
                (
                    &guards_.get(static_cast<Guards*>(nullptr)),
                    &event
                ) && ...
            );
        }

    private:
        detail::sm_object_holder_tuple<Guards...> guards_;
};

template<class... Guards>
class or_
{
    public:
        template<class Context, class Sm>
        or_(Context& context, Sm& machine):
            guards_(context, machine)
        {
        }

        template<class Event>
        bool check(const Event& event)
        {
            return
            (
                detail::call_check
                (
                    &guards_.get(static_cast<Guards*>(nullptr)),
                    &event
                ) || ...
            );
        }

    private:
        detail::sm_object_holder_tuple<Guards...> guards_;
};

template<class... Guards>
class xor_
{
    public:
        template<class Context, class Sm>
        xor_(Context& context, Sm& machine):
            guards_(context, machine)
        {
        }

        template<class Event>
        bool check(const Event& event)
        {
            return
            (
                detail::call_check
                (
                    &guards_.get(static_cast<Guards*>(nullptr)),
                    &event
                ) != ...
            );
        }

    private:
        detail::sm_object_holder_tuple<Guards...> guards_;
};

template<class Guard>
class not_
{
    public:
        template<class Context, class Sm>
        not_(Context& context, Sm& machine):
            guard_{context, machine}
        {
        }

        template<class Event>
        bool check(const Event& event)
        {
            return !detail::call_check(&guard_.object, &event);
        }

    private:
        detail::sm_object_holder<Guard> guard_;
};

} //namespace

#endif
