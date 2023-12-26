//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
{
    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct button_press
        {
        };
    }

    namespace states
    {
        EMPTY_STATE(off0)
        EMPTY_STATE(off1)
        EMPTY_STATE(on0)

        constexpr auto on1_transition_table = maki::transition_table_c
            .add_c<states::off0, events::button_press, states::on0>
        ;

        constexpr auto on1 = maki::submachine_conf_c
            .transition_tables(on1_transition_table)
            .pretty_name("on_1")
        ;
    }

    constexpr auto transition_table_0_t = maki::transition_table_c
        .add_c<states::off0, events::button_press, states::on0>
    ;

    constexpr auto transition_table_1_t = maki::transition_table_c
        .add_c<states::off1, events::button_press, states::on1>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::submachine_conf_c
            .transition_tables(transition_table_0_t, transition_table_1_t)
            .context<context>()
            .pretty_name("main_sm")
        ;

        context& ctx;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("path")
{
    {
        static constexpr auto path = maki::path{machine_def::conf} / 0;
        REQUIRE(maki::path_to_string(maki::cref_constant_c<path>) == "main_sm/0");
    }

    {
        static constexpr auto path = maki::path{machine_def::conf} / 1;
        REQUIRE(maki::path_to_string(maki::cref_constant_c<path>) == "main_sm/1");
    }

    {
        static constexpr auto path = maki::path{machine_def::conf} / 1 / states::on1 / 0;
        REQUIRE(maki::path_to_string(maki::cref_constant_c<path>) == "main_sm/1/on_1/0");
    }
}
