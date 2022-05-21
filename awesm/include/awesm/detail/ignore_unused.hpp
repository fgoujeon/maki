//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_IGNORE_UNUSED_HPP
#define AWESM_DETAIL_IGNORE_UNUSED_HPP

namespace awesm::detail
{

//Used to silence "Unused parameter" warnings
template<class... Args>
void ignore_unused(const Args&...)
{
}

} //namespace

#endif
