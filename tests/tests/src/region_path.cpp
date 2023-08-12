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
        EMPTY_STATE(off0);
        EMPTY_STATE(off1);
        EMPTY_STATE(on0);

        using on1_transition_table = maki::transition_table
            ::add<states::off0, events::button_press, states::on0>
        ;

        struct on1
        {
            using conf = maki::submachine_conf
                ::transition_tables<on1_transition_table>
                ::pretty_name
            ;

            static auto pretty_name()
            {
                return "on_1";
            }
        };
    }

    using transition_table_0_t = maki::transition_table
        ::add<states::off0, events::button_press, states::on0>
    ;

    using transition_table_1_t = maki::transition_table
        ::add<states::off1, events::button_press, states::on1>
    ;

    struct machine_def
    {
        using conf = maki::submachine_conf
            ::transition_tables<transition_table_0_t, transition_table_1_t>
            ::context<context>
            ::pretty_name
        ;

        static auto pretty_name()
        {
            return "main_sm";
        }

        context& ctx;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("region_path")
{
    {
        using region_path_t = maki::region_path<machine_def, 0>;
        REQUIRE(region_path_t::to_string() == "main_sm[0]");
    }

    {
        using region_path_t = maki::region_path<machine_def, 1>;
        REQUIRE(region_path_t::to_string() == "main_sm[1]");
    }

    {
        using region_path_t = maki::region_path<machine_def, 1>::add<states::on1, 0>;
        REQUIRE(region_path_t::to_string() == "main_sm[1].on_1");
    }

    {
        using region_path_t = maki::region_path_tpl
        <
            maki::region_path_element<machine_def, 1>,
            maki::region_path_element<states::on1, 0>
        >;

        using region_path_2_t = maki::region_path<machine_def, 1>::add<states::on1, 0>;

        REQUIRE(std::is_same_v<region_path_t, region_path_2_t>);
    }
}
