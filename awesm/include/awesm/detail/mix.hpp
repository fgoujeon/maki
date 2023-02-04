//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_MIX_HPP
#define AWESM_DETAIL_MIX_HPP

namespace awesm::detail
{

template<class... Ts>
struct mix: Ts...
{
};

} //namespace

#endif
