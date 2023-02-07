//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TYPE_TAG_HPP
#define AWESM_DETAIL_TYPE_TAG_HPP

namespace awesm::detail
{

//A type holder
template<class T>
struct type_tag
{
    using type = T;
};

} //namespace

#endif
