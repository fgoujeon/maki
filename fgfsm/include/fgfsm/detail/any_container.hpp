//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_ANY_CONTAINER_HPP
#define FGFSM_DETAIL_ANY_CONTAINER_HPP

#include <type_traits>
#include <memory>

namespace fgfsm::detail
{

//A minimal std::any-like container that fits our needs
class any_container
{
    public:
        template<class Object>
        explicit any_container(const Object& obj):
            pobj_
            (
                reinterpret_cast<const fake_void*>(new Object{obj}),
                [](const fake_void* pobj)
                {
                    delete reinterpret_cast<const Object*>(pobj);
                }
            )
        {
        }

        any_container(const any_container&) = delete;
        any_container(any_container&& other) = delete;
        ~any_container() = default;
        void operator=(const any_container&) = delete;
        void operator=(any_container&& other) = delete;

        template<class T>
        const T& get() const
        {
            return *reinterpret_cast<const T*>(pobj_.get());
        }

    private:
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
        using unique_ptr_to_const_fake_void = std::unique_ptr
        <
            const fake_void,
            void(*)(const fake_void*)
        >;

        unique_ptr_to_const_fake_void pobj_;
};

} //namespace

#endif
