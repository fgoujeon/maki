//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_EVENT_REF_HPP
#define FGFSM_EVENT_REF_HPP

namespace fgfsm
{

class event_ref
{
    private:
        template<class T>
        struct manager
        {
            static void identifier()
            {
            }
        };

        using identifier_fn = void(*)();

    public:
        template<class T>
        event_ref(const T& evt):
            evt_(&evt),
            identifier_(&manager<T>::identifier)
        {
        }

        template<class T>
        const T* get() const
        {
            if(identifier_ == &manager<T>::identifier)
                return static_cast<const T*>(evt_);
            else
                return nullptr;
        }

        template<class Event>
        bool is() const
        {
            return get<Event>() != nullptr;
        }

    private:
        const void* evt_;
        identifier_fn identifier_;
};

} //namespace

#endif
