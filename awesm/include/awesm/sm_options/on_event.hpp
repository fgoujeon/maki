//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_OPTIONS_ON_EVENT_HPP
#define AWESM_SM_OPTIONS_ON_EVENT_HPP

namespace awesm::sm_options
{

namespace detail::tags
{
    struct on_event{};
}

namespace detail::defaults
{
    struct on_event
    {
        template<class Sm, class Context>
        on_event(Sm& /*sm*/, Context& /*ctx*/)
        {
        }

        void call(tags::on_event /*tag*/, const void* /*pevent*/, long /*priority*/)
        {
        }
    };
}

template<class T>
class on_event
{
    public:
        template<class Sm, class Context>
        on_event(Sm& /*sm*/, Context& /*ctx*/)
        {
        }

        template<class Event>
        void call(detail::tags::on_event /*tag*/, const Event* pevent, int /*priority*/)
        {
            impl_.on_event(*pevent);
        }

    private:
        T impl_;
};

} //namespace

#endif
