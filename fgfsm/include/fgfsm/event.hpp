//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_EVENT_REF_HPP
#define FGFSM_EVENT_REF_HPP

#include <type_traits>
#include <utility>

namespace fgfsm
{

class event_ref;

//Stores an instance of any type
class event
{
    private:
        template<class Event>
        struct manager
        {
            static void free(const void* pevt)
            {
                delete static_cast<const Event*>(pevt);
            }

            static event_ref make_ref(const void* pevt);
        };

        using free_fn = void(*)(const void*);
        using make_ref_fn = event_ref(*)(const void*);

    public:
        template<class Event>
        event(const Event& evt):
            pevt_(new Event{evt}),
            free_(&manager<Event>::free),
            make_ref_(&manager<Event>::make_ref)
        {
        }

        event(const event&) = delete;

        event(event&& other):
            pevt_(std::exchange(other.pevt_, nullptr)),
            free_(other.free_),
            make_ref_(other.make_ref_)
        {
        }

        event(const event_ref&) = delete;

        ~event()
        {
            free_(pevt_);
        }

        event_ref make_ref() const;

    private:
        const void* pevt_;
        free_fn free_;
        make_ref_fn make_ref_;
};

//Holds a reference to any type
class event_ref
{
    private:
        template<class Event>
        struct manager
        {
            static event make_deep_copy(const void* pevt)
            {
                const auto pevt2 = static_cast<const Event*>(pevt);
                return event{*pevt2};
            }
        };

        using make_deep_copy_fn = event(*)(const void*);

    public:
        template<class Event>
        event_ref(const Event& evt):
            pevt_(&evt),
            make_deep_copy_(&manager<Event>::make_deep_copy)
        {
        }

        event_ref(const event_ref& evt):
            pevt_(evt.pevt_),
            make_deep_copy_(evt.make_deep_copy_)
        {
        }

        event_ref(const event&) = delete;

        template<class OtherEvent>
        const OtherEvent* get() const
        {
            if(make_deep_copy_ == &manager<OtherEvent>::make_deep_copy) //Event == OtherEvent?
                return static_cast<const OtherEvent*>(pevt_);
            else
                return nullptr;
        }

        template<class OtherEvent>
        bool is() const
        {
            return get<OtherEvent>() != nullptr;
        }

        event make_deep_copy() const
        {
            return make_deep_copy_(pevt_);
        }

    private:
        const void* pevt_;
        make_deep_copy_fn make_deep_copy_;
};

template<class Event>
event_ref event::manager<Event>::make_ref(const void* pevt)
{
    return event_ref{*static_cast<const Event*>(pevt)};
}

inline
event_ref event::make_ref() const
{
    return make_ref_(pevt_);
}

} //namespace

#endif
