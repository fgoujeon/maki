//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_OPTIONS_HPP
#define AWESM_SM_OPTIONS_HPP

namespace awesm::sm_options
{

struct after_state_transition{};
struct before_entry{};
struct before_state_transition{};
struct disable_run_to_completion{};
struct on_event{};
struct on_exception{};

} //namespace

#endif
