//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REF_WRAPPER_HPP
#define MAKI_DETAIL_REF_WRAPPER_HPP

namespace maki::detail
{

/*
Stores a `T&`.
*/
template<class T>
class ref_wrapper
{
public:
    constexpr ref_wrapper(const T& obj):
        ptr_(&obj)
    {
    }

    constexpr ref_wrapper(const ref_wrapper&) = default;

    constexpr ref_wrapper(ref_wrapper&&) noexcept = default;

    ~ref_wrapper() = default;

    ref_wrapper& operator=(const ref_wrapper&) = delete;

    ref_wrapper& operator=(ref_wrapper&&) = delete;

    constexpr const T& get() const
    {
        return *ptr_;
    }

    constexpr const T* get_as_ptr() const
    {
        return ptr_;
    }

private:
    const T* ptr_ = nullptr;
};

template<class T>
constexpr bool operator==(const ref_wrapper<T>& lhs, const ref_wrapper<T>& rhs)
{
    return lhs.get_as_ptr() == rhs.get_as_ptr();
}

template<class T, class U>
constexpr bool operator==(const ref_wrapper<T>& /*lhs*/, const ref_wrapper<U>& /*rhs*/)
{
    return false;
}

template<class T>
constexpr auto make_cref_wrapper(const T& obj)
{
    return ref_wrapper<const T>{obj};
}

} //namespace

#endif
