//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_FAKE_FUNCTION_QUEUE_HPP
#define AWESM_DETAIL_FAKE_FUNCTION_QUEUE_HPP

namespace awesm::detail
{

/*
Mimics the interface of function_queue but does absolutely nothing
*/
template<class Arg>
class fake_function_queue
{
public:
    template<class FunHolder, class Data>
    void push(const Data& /*data*/)
    {
    }

    void invoke_and_pop_all(Arg /*arg*/)
    {
    }
};

} //namespace

#endif
