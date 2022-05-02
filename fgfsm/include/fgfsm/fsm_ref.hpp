//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_REF_HPP
#define FGFSM_FSM_REF_HPP

namespace fgfsm
{

template<class... Events>
class fsm_ref;

template<class Event, class... Events>
class fsm_ref<Event, Events...>: fsm_ref<Events...>
{
    public:
        template<class Fsm>
        fsm_ref(Fsm& sm):
            fsm_ref<Events...>{sm},
            pprocess_event_
            {
                [](void* const vpfsm, const Event& event)
                {
                    const auto pfsm = reinterpret_cast<Fsm*>(vpfsm);
                    pfsm->process_event(event);
                }
            }
        {
        }

        using fsm_ref<Events...>::process_event;

        void process_event(const Event& event)
        {
            (*pprocess_event_)(this->vpfsm_, event);
        }

    private:
        using pprocess_event_t = void(*)(void*, const Event&);
        pprocess_event_t pprocess_event_ = nullptr;
};

template<>
class fsm_ref<>
{
    public:
        template<class Fsm>
        fsm_ref(Fsm& sm):
            vpfsm_(&sm)
        {
        }

        void process_event()
        {
        }

    protected:
        void* vpfsm_ = nullptr;
};

} //namespace

#endif
