//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_DEFAULT_EXCEPTION_HANDLER_HPP
#define AWESM_DETAIL_DEFAULT_EXCEPTION_HANDLER_HPP

#include "../sm_ref.hpp"
#include <exception>

namespace awesm::detail
{

class default_exception_handler
{
    public:
        template<class Sm, class Context>
        default_exception_handler(Sm& m, Context& /*ctx*/):
            sm_(m)
        {
        }

        void on_exception(const std::exception_ptr& e)
        {
            sm_.process_event(e);
        }

    private:
        sm_ref<std::exception_ptr> sm_;
};

} //namespace

#endif
