//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_REF_E_HPP
#define AWESM_SM_REF_E_HPP

#include "sm_ref_conf.hpp"
#include "sm_ref.hpp"

namespace awesm
{

template<class... Events>
struct sm_ref_e_def
{
    using conf = sm_ref_conf
        ::events<Events...>
    ;
};

template<class... Events>
using sm_ref_e = sm_ref<sm_ref_e_def<Events...>>;

} //namespace

#endif
