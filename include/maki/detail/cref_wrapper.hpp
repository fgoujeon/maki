//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CREF_WRAPPER_HPP
#define MAKI_DETAIL_CREF_WRAPPER_HPP

namespace maki::detail
{

/*
Stores a `const T&`.
*/
template<class T>
class cref_wrapper
{
public:
    constexpr cref_wrapper(const T& obj):
        ptr_(&obj)
    {
    }

    constexpr cref_wrapper(const cref_wrapper&) = default;

    constexpr cref_wrapper(cref_wrapper&&) noexcept = default;

    ~cref_wrapper() = default;

    cref_wrapper& operator=(const cref_wrapper&) = delete;

    cref_wrapper& operator=(cref_wrapper&&) = delete;

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
constexpr bool operator==(const cref_wrapper<T>& lhs, const cref_wrapper<T>& rhs)
{
    return lhs.get_as_ptr() == rhs.get_as_ptr();
}

template<class T, class U>
constexpr bool operator==(const cref_wrapper<T>& /*lhs*/, const cref_wrapper<U>& /*rhs*/)
{
    return false;
}

template<class T>
constexpr auto make_cref_wrapper(const T& obj)
{
    return cref_wrapper<T>{obj};
}

} //namespace

#endif
