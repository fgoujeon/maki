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

        constexpr auto on1_transition_table = maki::empty_transition_table
            .add_c<states::off0, events::button_press, states::on0>
        ;

        constexpr auto on1 = maki::submachine_conf
            .transition_tables(on1_transition_table)
            .pretty_name("on_1")
        ;
    }

    constexpr auto transition_table_0_t = maki::empty_transition_table
        .add_c<states::off0, events::button_press, states::on0>
    ;

    constexpr auto transition_table_1_t = maki::empty_transition_table
        .add_c<states::off1, events::button_press, states::on1>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::submachine_conf
            .transition_tables(transition_table_0_t, transition_table_1_t)
            .context<context>()
            .pretty_name("main_sm")
        ;

        context& ctx;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("region_path")
{
    {
        constexpr auto region_path = maki::region_path_c<machine_def::conf, 0>;
        REQUIRE(region_path.to_string() == "main_sm[0]");
    }

    {
        constexpr auto region_path = maki::region_path_c<machine_def::conf, 1>;
        REQUIRE(region_path.to_string() == "main_sm[1]");
    }

    {
        constexpr auto region_path = maki::region_path_c<machine_def::conf, 1>.add<states::on1, 0>();
        REQUIRE(region_path.to_string() == "main_sm[1].on_1");
    }

    {
        constexpr auto region_path = maki::region_path
        <
            maki::region_path_element<machine_def::conf, 1>,
            maki::region_path_element<states::on1, 0>
        >{};

        constexpr auto region_path_2 = maki::region_path_c<machine_def::conf, 1>.add<states::on1, 0>();

        REQUIRE(std::is_same_v<decltype(region_path), decltype(region_path_2)>);
    }
}
