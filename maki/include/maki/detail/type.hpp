//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TYPE_TAG_HPP
#define MAKI_DETAIL_TYPE_TAG_HPP

namespace maki::detail
{

//A type holder
template<class T>
struct type_t
{
    using type = T;
};

} //namespace

#endif
