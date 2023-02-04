//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef COMMON_EMPTY_STATE_HPP
#define COMMON_EMPTY_STATE_HPP

#define EMPTY_STATE(name) \
    struct name \
    { \
        using conf = awesm::state_conf<>; \
    }

#endif
