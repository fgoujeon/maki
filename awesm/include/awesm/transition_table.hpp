//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_TRANSITION_TABLE_HPP
#define AWESM_TRANSITION_TABLE_HPP

#include "row.hpp"
#include "none.hpp"
#include "detail/tlu/at_or.hpp"
#include "detail/tlu/at.hpp"

namespace awesm
{

namespace detail
{
    template<class Row>
    using tt_row_source_state = tlu::at<Row, 0>;

    template<class Row>
    using tt_row_event = tlu::at<Row, 1>;

    template<class Row>
    using tt_row_target_state = tlu::at<Row, 2>;

    template<class Row>
    using tt_row_action = tlu::at_or<Row, 3, none>;

    template<class Row>
    using tt_row_guard = tlu::at_or<Row, 4, none>;
}

template<class... Ts>
struct transition_table{};

} //namespace

#endif
