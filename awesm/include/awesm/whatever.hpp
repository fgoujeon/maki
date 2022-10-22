//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_WHATEVER_HPP
#define AWESM_WHATEVER_HPP

namespace awesm
{

//Used as function parameter when type and value don't matter.
struct whatever
{
    template<class T>
    whatever(const T& /*ignored*/)
    {
    }
};

} //namespace

#endif
