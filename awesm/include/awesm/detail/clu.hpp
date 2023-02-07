//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

/*
CLU = Constant List Utility

A constant list is an instance of a type template of this form:
    template<auto... Cs>
    struct constant_list
    {
    };
*/

#include "clu/apply.hpp"
#include "clu/size.hpp"
