//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_PATH_OF_HPP
#define MAKI_DETAIL_REGION_PATH_OF_HPP

namespace maki::detail
{

//Must be specialized by each type
template<class T>
struct region_path_of;

template<class T>
using region_path_of_t = typename region_path_of<T>::type;

//Must be specialized by each type
template<class T>
struct machine_of;

template<class T>
using root_sm_of_t = typename machine_of<T>::type;

} //namespace

#endif
