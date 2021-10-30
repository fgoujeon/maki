//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_EVENT_REF_HPP
#define FGFSM_EVENT_REF_HPP

#include <type_traits>
#include <memory>
#include <utility>

namespace fgfsm
{

namespace detail
{
    /*
    A way to have a valid std::unique_ptr<const void, void(*)(const void*)>.
    We still have to reinterpret_cast instead of static_cast-ing from/to
    fake_void, though.
    */
    struct fake_void
    {
        fake_void() = delete;
        fake_void(const fake_void&) = delete;
        fake_void(fake_void&&) = delete;
    };
    using unique_ptr_to_const_fake_void = std::unique_ptr<const fake_void, void(*)(const fake_void*)>;
}

class event_ref;

//Stores an instance of any type
class event
{
    private:
        friend class event_ref;

        template<class Event>
        struct manager
        {
            static void free(const detail::fake_void* pevt)
            {
                delete reinterpret_cast<const Event*>(pevt);
            }

            static event_ref make_ref(const detail::fake_void* pevt);
        };

        using make_ref_fn = event_ref(*)(const detail::fake_void*);

    public:
        template<class Event>
        event(const Event& evt):
            pevt_
            (
                reinterpret_cast<const detail::fake_void*>(new Event{evt}),
                &manager<Event>::free
            ),
            make_ref_(&manager<Event>::make_ref)
        {
        }

        event(const event&) = delete;

        event(event&& other):
            pevt_(std::move(other.pevt_)),
            make_ref_(other.make_ref_)
        {
        }

        event(const event_ref& evt);

        void operator=(const event&) = delete;

        void operator=(event&& other)
        {
            pevt_ = std::move(other.pevt_);
            make_ref_ = std::exchange(other.make_ref_, nullptr);
        }

    private:
        event_ref make_ref() const;

    private:
        detail::unique_ptr_to_const_fake_void pevt_;
        make_ref_fn make_ref_ = nullptr;
};

//Holds a reference to any type
class event_ref
{
    private:
        friend class event;

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

        event_ref(const event_ref& other):
            pevt_(other.pevt_),
            make_deep_copy_(other.make_deep_copy_)
        {
        }

        event_ref(const event& evt)
        {
            *this = evt.make_ref();
        }

        event_ref& operator=(const event_ref&) = default;

        event_ref& operator=(event_ref&&) = default;

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

    private:
        event make_deep_copy() const
        {
            return make_deep_copy_(pevt_);
        }

    private:
        const void* pevt_ = nullptr;
        make_deep_copy_fn make_deep_copy_;
};

template<class Event>
event_ref event::manager<Event>::make_ref(const detail::fake_void* pevt)
{
    return event_ref{*reinterpret_cast<const Event*>(pevt)};
}

inline
event::event(const event_ref& evt):
    pevt_(nullptr, nullptr)
{
    *this = evt.make_deep_copy();
}

inline
event_ref event::make_ref() const
{
    return make_ref_(pevt_.get());
}

} //namespace

#endif
