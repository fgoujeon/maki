//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
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

        struct exception_request
        {
        };
    }

    namespace states
    {
        EMPTY_STATE(off0);
        EMPTY_STATE(off1);
        EMPTY_STATE(on0);

        struct on1
        {
            using conf_type = awesm::composite_state_conf
            <
                awesm::transition_table
                <
                    awesm::row<states::off0, events::button_press, states::on0>
                >,
                awesm::state_options::get_pretty_name
            >;

            static auto get_pretty_name()
            {
                return "on_1";
            }
        };
    }

    struct sm_def
    {
        using conf_type = awesm::sm_conf
        <
            awesm::transition_table_list
            <
                awesm::transition_table
                <
                    awesm::row<states::off0, events::button_press, states::on0>
                >,
                awesm::transition_table
                <
                    awesm::row<states::off1, events::button_press, states::on1>
                >
            >,
            context,
            awesm::sm_options::get_pretty_name
        >;

        static auto get_pretty_name()
        {
            return "main_sm";
        }

        context& ctx;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("region_path")
{
    {
        using region_path_t = awesm::region_path
        <
            awesm::region_path_element<sm_t, 0>
        >;

        REQUIRE(awesm::get_pretty_name<region_path_t>() == "main_sm[0]");
    }

    {
        using region_path_t = awesm::region_path
        <
            awesm::region_path_element<sm_t, 1>
        >;

        REQUIRE(awesm::get_pretty_name<region_path_t>() == "main_sm[1]");
    }

    {
        using region_path_t = awesm::region_path
        <
            awesm::region_path_element<sm_t, 1>,
            awesm::region_path_element<states::on1, 0>
        >;

        REQUIRE(region_path_t::get_pretty_name() == "main_sm[1].on_1");
    }
}
