//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_FUNCTION_QUEUE_HPP
#define MAKI_DETAIL_FUNCTION_QUEUE_HPP

#include <queue>
#include <cstddef>

namespace maki::detail
{

/*
A kind of std::queue<std::function<void(Arg)>>, optimized for our needs
*/
template
<
    class Arg,
    std::size_t StaticStorageSize,
    std::size_t StaticStorageAlignment = alignof(std::max_align_t)
>
class function_queue
{
public:
    //Push call to FunHolder::call(data, arg)
    template<class FunHolder, class Data>
    void push(const Data& data)
    {
        if constexpr(std::is_nothrow_copy_constructible_v<Data>)
        {
            queue_.emplace(&call<Data, FunHolder>, &delete_data<Data>).set_data(data);
        }
        else
        {
            /*
            First make pdelete point to dont_delete_data() (which doesn't do
            anything) in case the Data copy constructor throws.
            Once the data is copied, we can safely make pdelete point to
            delete_data().
            */

            auto& cont = queue_.emplace(&call<Data, FunHolder>);
            cont.set_data(data);
            cont.set_delete(&delete_data<Data>);
        }
    }

    void invoke_and_pop_all(Arg arg)
    {
        while(!queue_.empty())
        {
            queue_.front().call(arg);
            queue_.pop();
        }
    }

    void clear()
    {
        std::queue<data_container>{}.swap(queue_);
    }

private:
    using call_fn_ptr_t = void (*)(const void*, Arg);
    using delete_fn_ptr_t = void (*)(const void*);

    /*
    A container for an object of any type, with small object optimization.

    The interface is designed so that the types of the arguments given to
    queue_.emplace() don't change, whatever FunHolder and Data are.
    This makes build time shorter and binary smaller.
    */
    struct data_container
    {
        //To be called when Data copy constructor can throw
        data_container //NOLINT
        (
            const call_fn_ptr_t pcall_
        ):
            pcall_(pcall_)
        {
        }

        //To be called when Data copy constructor cannot throw
        data_container //NOLINT
        (
            const call_fn_ptr_t pcall_,
            const delete_fn_ptr_t pdelete_
        ):
            pcall_(pcall_),
            pdelete_(pdelete_)
        {
        }

        data_container(const data_container&) = delete;
        data_container(data_container&& other) = delete;

        ~data_container()
        {
            pdelete_(pdata_);
        }

        void operator=(const data_container&) = delete;
        void operator=(data_container&& other) = delete;

        template<class Data>
        void set_data(const Data& data)
        {
            //Copy data into the data_container
            if constexpr(suitable_for_static_storage<Data>())
            {
                pdata_ = new(static_storage_) Data{data}; //NOLINT
            }
            else
            {
                pdata_ = new Data{data}; //NOLINT
            }
        }

        void set_delete(const delete_fn_ptr_t pdelete)
        {
            pdelete_ = pdelete;
        }

        void call(Arg arg)
        {
            pcall_(pdata_, arg);
        }

    private:
        //Storage for small object optimization, properly aligned for an object
        //whose alignment requirement is less than or equal to
        //StaticStorageAlignment
        alignas(StaticStorageAlignment) char static_storage_[StaticStorageSize]; //NOLINT

        void* pdata_; //NOLINT
        call_fn_ptr_t pcall_ = nullptr;
        delete_fn_ptr_t pdelete_ = &dont_delete_data;
    };

    template<class Data>
    static constexpr bool suitable_for_static_storage()
    {
        return
            sizeof(Data) <= StaticStorageSize &&
            alignof(Data) <= StaticStorageAlignment
        ;
    }

    template<class Data, class FunHolder>
    static void call(const void* const pdata, Arg arg)
    {
        const Data& data = *reinterpret_cast<const Data*>(pdata); //NOLINT
        FunHolder::call(data, arg);
    }

    static void dont_delete_data(const void* const /*pdata*/)
    {
    }

    template<class Data>
    static void delete_data(const void* const pdata)
    {
        if constexpr(suitable_for_static_storage<Data>())
        {
            reinterpret_cast<const Data*>(pdata)->~Data(); //NOLINT
        }
        else
        {
            delete reinterpret_cast<const Data*>(pdata); //NOLINT
        }
    }

    std::queue<data_container> queue_;
};

} //namespace

#endif
