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

//Stores a copy of the given object
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

        using make_ref_t = event_ref(*)(const detail::fake_void*);

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
        make_ref_t make_ref_ = nullptr;
};

//Holds a reference to the given object
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

            /*
            ID unique to each Event type.
            The ID is not the value of the variable, but its address.
            */
            static constexpr char identifier = {};
        };

        using identifier_t = const char*;
        using make_deep_copy_t = event(*)(const void*);

    public:
        template<class Event>
        event_ref(const Event& evt):
            pevt_(&evt),
            identifier_(&manager<Event>::identifier),
            make_deep_copy_(&manager<Event>::make_deep_copy)
        {
        }

        event_ref(const event_ref& other):
            pevt_(other.pevt_),
            identifier_(other.identifier_),
            make_deep_copy_(other.make_deep_copy_)
        {
        }

        event_ref(const event& evt)
        {
            *this = evt.make_ref();
        }

    private:
        event_ref& operator=(const event_ref&) = default;

        event_ref& operator=(event_ref&&) = default;

    public:
        template<class OtherEvent>
        const OtherEvent* get() const
        {
            if(identifier_ == &manager<OtherEvent>::identifier) //Event == OtherEvent?
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
        identifier_t identifier_;
        make_deep_copy_t make_deep_copy_;
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



//
//visit
//

namespace detail
{
    //Fonction object case
    template<class F>
    struct first_arg_of_unary_function:
        first_arg_of_unary_function<decltype(&F::operator())>
    {
    };

    //Fonction case
    template<class R, class Arg>
    struct first_arg_of_unary_function<R(Arg)>
    {
        using type = Arg;
    };

    //Fonction pointer case
    template<class R, class Arg>
    struct first_arg_of_unary_function<R(*)(Arg)>
    {
        using type = Arg;
    };

    //Pointer to member function case
    template<class T, class R, class Arg>
    struct first_arg_of_unary_function<R(T::*)(Arg)>
    {
        using type = Arg;
    };

    //Pointer to member function case
    template<class T, class R, class Arg>
    struct first_arg_of_unary_function<R(T::*)(Arg) const>
    {
        using type = Arg;
    };

    template<class F>
    using first_arg_of_unary_function_t =
        typename first_arg_of_unary_function<F>::type
    ;
}

template<class Visitor, class... Visitors>
void visit(const event_ref& evt, Visitor&& visitor, Visitors&&... visitors)
{
    using decayed_visitor_t = std::decay_t<Visitor>;
    using arg_type_t = detail::first_arg_of_unary_function_t<decayed_visitor_t>;
    using decayed_arg_type_t = std::decay_t<arg_type_t>;

    if(const auto pevt = evt.get<decayed_arg_type_t>())
        visitor(*pevt);
    else
        visit(evt, visitors...);
}

inline
void visit(const event_ref&)
{
}

} //namespace

#endif
