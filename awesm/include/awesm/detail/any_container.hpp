//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_ANY_CONTAINER_HPP
#define AWESM_DETAIL_ANY_CONTAINER_HPP

#include <type_traits>
#include <memory>

namespace awesm::detail
{

/*
A type-unsafe container for an object of any type.
No dynamic memory allocation happens if the size of the given object is lower
or equal to StaticStorageSize.
*/
template<auto StaticStorageSize>
class any_container
{
    public:
        static_assert(StaticStorageSize > 0);

        //small object optimization
        template<class Object>
        explicit any_container
        (
            const Object& obj,
            std::enable_if_t<sizeof(Object) <= StaticStorageSize>* /*pvoid*/ = nullptr
        ):
            pobj_(new(static_storage_) Object{obj}),
            pdelete_
            (
                [](const void* pobj)
                {
                    reinterpret_cast<const Object*>(pobj)->~Object(); //NOLINT
                }
            )
        {
        }

        template<class Object>
        explicit any_container
        (
            const Object& obj,
            std::enable_if_t<(sizeof(Object) > StaticStorageSize)>* /*pvoid*/ = nullptr
        ):
            pobj_(new Object{obj}),
            pdelete_
            (
                [](const void* pobj)
                {
                    delete reinterpret_cast<const Object*>(pobj); //NOLINT
                }
            )
        {
        }

        any_container(const any_container&) = delete;
        any_container(any_container&& other) = delete;

        ~any_container()
        {
            (*pdelete_)(pobj_);
        }

        void operator=(const any_container&) = delete;
        void operator=(any_container&& other) = delete;

        [[nodiscard]]
        void* get()
        {
            return pobj_;
        }

        [[nodiscard]]
        const void* get() const
        {
            return pobj_;
        }

    private:
        char static_storage_[StaticStorageSize]; //NOLINT, small object optimization
        void* pobj_ = nullptr;
        void(*pdelete_)(const void*) = nullptr;
};

} //namespace

#endif
