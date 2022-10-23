//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_CONFIGURATION_HPP
#define AWESM_DETAIL_SM_CONFIGURATION_HPP

#include "sm_object_holder.hpp"
#include "../sm_options.hpp"

namespace awesm::detail
{

template<class... Options>
class sm_configuration: private Options...
{
    public:
        template<class Sm, class Context>
        sm_configuration(Sm& mach, Context& ctx):
            Options(mach, ctx)...
        {
        }
};

} //namespace

#endif
