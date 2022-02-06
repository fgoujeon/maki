//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_ANY_COPY_HPP
#define FGFSM_ANY_COPY_HPP

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

class any_cref;

//Stores a copy of the given object (like std::any)
class any_copy
{
    private:
        friend class any_cref;

        template<class Object>
        struct manager
        {
            static void free(const detail::fake_void* pevt)
            {
                delete reinterpret_cast<const Object*>(pevt);
            }

            static any_cref make_cref(const detail::fake_void* pevt);
        };

        using make_cref_t = any_cref(*)(const detail::fake_void*);

    public:
        template<class Object>
        explicit any_copy(const Object& evt):
            pevt_
            (
                reinterpret_cast<const detail::fake_void*>(new Object{evt}),
                &manager<Object>::free
            ),
            make_cref_(&manager<Object>::make_cref)
        {
        }

        any_copy(const any_copy&) = delete;

        any_copy(any_copy&& other):
            pevt_(std::move(other.pevt_)),
            make_cref_(other.make_cref_)
        {
        }

        explicit any_copy(const any_cref& evt);

        void operator=(const any_copy&) = delete;

        void operator=(any_copy&& other)
        {
            pevt_ = std::move(other.pevt_);
            make_cref_ = std::exchange(other.make_cref_, nullptr);
        }

    private:
        any_cref make_cref() const;

    private:
        detail::unique_ptr_to_const_fake_void pevt_;
        make_cref_t make_cref_ = nullptr;
};

//Stores a const reference to the given object (like what a std::any for const
//references would be)
class any_cref
{
    private:
        friend class any_copy;

        template<class T>
        struct manager
        {
            static any_copy make_copy(const void* pevt)
            {
                const auto pevt2 = static_cast<const T*>(pevt);
                return any_copy{*pevt2};
            }

            /*
            ID unique to each type.
            The ID is not the value of the variable, but its address.
            */
            static constexpr char identifier = {};
        };

        using identifier_t = const char*;
        using make_copy_t = any_copy(*)(const void*);

    public:
        template<class Object>
        any_cref(const Object& evt):
            pevt_(&evt),
            identifier_(&manager<Object>::identifier),
            make_copy_(&manager<Object>::make_copy)
        {
        }

        any_cref(const any_cref& other):
            pevt_(other.pevt_),
            identifier_(other.identifier_),
            make_copy_(other.make_copy_)
        {
        }

        any_cref(const any_copy& evt)
        {
            *this = evt.make_cref();
        }

    private:
        any_cref& operator=(const any_cref&) = default;

        any_cref& operator=(any_cref&&) = default;

    public:
        template<class T>
        const T* get_if() const
        {
            if(identifier_ == &manager<T>::identifier) //Object == T?
                return static_cast<const T*>(pevt_);
            else
                return nullptr;
        }

        template<class T>
        bool is() const
        {
            return get_if<T>() != nullptr;
        }

    private:
        any_copy make_copy() const
        {
            return make_copy_(pevt_);
        }

    private:
        const void* pevt_ = nullptr;
        identifier_t identifier_;
        make_copy_t make_copy_;
};

template<class Object>
any_cref any_copy::manager<Object>::make_cref(const detail::fake_void* pevt)
{
    return any_cref{*reinterpret_cast<const Object*>(pevt)};
}

inline
any_copy::any_copy(const any_cref& evt):
    pevt_(nullptr, nullptr)
{
    *this = evt.make_copy();
}

inline
any_cref any_copy::make_cref() const
{
    return make_cref_(pevt_.get());
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
void visit(const any_cref& evt, Visitor&& visitor, Visitors&&... visitors)
{
    using decayed_visitor_t = std::decay_t<Visitor>;
    using arg_type_t = detail::first_arg_of_unary_function_t<decayed_visitor_t>;
    using decayed_arg_type_t = std::decay_t<arg_type_t>;

    if(const auto pevt = evt.get_if<decayed_arg_type_t>())
        visitor(*pevt);
    else
        visit(evt, visitors...);
}

inline
void visit(const any_cref&)
{
}

} //namespace

#endif
