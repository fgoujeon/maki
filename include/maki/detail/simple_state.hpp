//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_HPP
#define MAKI_DETAIL_SIMPLE_STATE_HPP

namespace maki::detail
{

/*
Implementation of a non-composite state
*/
template<class ConfHolder>
struct simple_state
{
    static constexpr const auto& conf = ConfHolder::conf;
};

} //namespace

#endif
