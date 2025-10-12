//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_OVERLOAD_HPP
#define MAKI_DETAIL_OVERLOAD_HPP

namespace maki::detail
{

template<class... Fs>
struct overload: Fs...
{
    using Fs::operator()...;
    static constexpr auto size = sizeof...(Fs);
};

template<class... Fs>
overload(Fs...) -> overload<Fs...>;

template<class... Fs, class F>
constexpr overload<Fs..., F> add(const overload<Fs...>& ovl, const F& func)
{
    return overload{static_cast<const Fs&>(ovl)..., func};
}

} //namespace

#endif
