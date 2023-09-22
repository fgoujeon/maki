//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_NOOP_HPP
#define MAKI_NOOP_HPP

namespace maki
{

/**
@brief An action that does nothing.
*/
struct noop_t
{
    template<class... Args>
    constexpr void operator()(const Args&... /*args*/) const
    {
    }
};

inline constexpr auto noop = noop_t{};

} //namespace

#endif
