//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FINAL_ACT_HPP
#define FGFSM_FINAL_ACT_HPP

namespace fgfsm::detail
{

//Calls f() at destruction
template<class F>
class final_act
{
    public:
        final_act(F&& f):
            f_(std::forward<F>(f))
        {
        }

        final_act(const final_act&) = delete;
        final_act(final_act&&) = delete;
        final_act& operator=(const final_act&) = delete;
        final_act& operator=(final_act&&) = delete;

        ~final_act()
        {
            f_();
        }

    private:
        F f_;
};

template<class F>
final_act(F&& f) -> final_act<F>;

} //namespace

#endif
