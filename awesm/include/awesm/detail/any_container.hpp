//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_ANY_CONTAINER_HPP
#define AWESM_DETAIL_ANY_CONTAINER_HPP

#include "type_tag.hpp"
#include <cstddef>

namespace awesm::detail
{

/*
A type-safe container for an object of any type, with small object optimization.
*/
template
<
    class Arg,
    std::size_t StaticStorageSize,
    std::size_t StaticStorageAlignment = alignof(std::max_align_t)
>
class any_container
{
    public:
        template<class Object, class Visitor>
        explicit any_container(const Object& obj, type_tag<Visitor> /*unused*/):
            pobj_(copy_object(obj)),
            pvisit_(&visit_impl<Object, Visitor>),
            pdelete_(&delete_object<Object>)
        {
        }

        any_container(const any_container&) = delete;
        any_container(any_container&& other) = delete;

        ~any_container()
        {
            pdelete_(pobj_);
        }

        void operator=(const any_container&) = delete;
        void operator=(any_container&& other) = delete;

        void visit(Arg arg) const
        {
            pvisit_(pobj_, arg);
        }

    private:
        template<class Object>
        void* copy_object(const Object& obj)
        {
            if constexpr(suitable_for_static_storage<Object>())
            {
                return new(static_storage_) Object{obj}; //NOLINT
            }
            else
            {
                return new Object{obj}; //NOLINT
            }
        }

        template<class Object>
        static void delete_object(const void* const pobj)
        {
            if constexpr(suitable_for_static_storage<Object>())
            {
                reinterpret_cast<const Object*>(pobj)->~Object(); //NOLINT
            }
            else
            {
                delete reinterpret_cast<const Object*>(pobj); //NOLINT
            }
        }

        template<class Object, class Visitor>
        static void visit_impl(const void* const pobj, Arg arg)
        {
            const Object& obj = *reinterpret_cast<const Object*>(pobj); //NOLINT
            Visitor::call(obj, arg);
        }

        template<class Object>
        static constexpr bool suitable_for_static_storage()
        {
            return
                sizeof(Object) <= StaticStorageSize &&
                alignof(Object) <= StaticStorageAlignment
            ;
        }

        //Storage for small object optimization, properly aligned for an object
        //whose alignment requirement is less than or equal to
        //StaticStorageAlignment
        alignas(StaticStorageAlignment) char static_storage_[StaticStorageSize]; //NOLINT

        void* pobj_ = nullptr;
        void(*pvisit_)(const void*, Arg) = nullptr;
        void(*pdelete_)(const void*) = nullptr;
};

} //namespace

#endif
