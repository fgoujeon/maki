//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace pretty_name_ns
{
    struct test{};

    template<class T, class U>
    class templ{};

    struct has_get_pretty_name
    {
        static auto get_pretty_name()
        {
            return "custom_name";
        }
    };

    struct state
    {
        using conf = awesm::state_conf
        <
            awesm::state_opts::get_pretty_name
        >;

        static const char* get_pretty_name()
        {
            return "my_state";
        }
    };

    auto subsm_transition_table()
    {
        return awesm::transition_table
            .add<state, awesm::null, awesm::null>
        ;
    }

    struct subsm
    {
        using conf = awesm::subsm_conf
        <
            subsm_transition_table,
            awesm::subsm_opts::get_pretty_name
        >;

        static const char* get_pretty_name()
        {
            return "my_subsm";
        }
    };

    struct context
    {
    };

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<state, awesm::null, awesm::null>
        ;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            sm_transition_table,
            context,
            awesm::sm_opts::get_pretty_name
        >;

        static const char* get_pretty_name()
        {
            return "my_sm";
        }
    };

    using sm_t = awesm::sm<sm_def>;

    struct region_path{};
}

TEST_CASE("pretty_name")
{
    REQUIRE
    (
        awesm::get_pretty_name<pretty_name_ns::test>() ==
        std::string_view{"test"}
    );

    REQUIRE
    (
        awesm::get_pretty_name<pretty_name_ns::templ<int, pretty_name_ns::test>>() ==
        std::string_view{"templ"}
    );

    REQUIRE
    (
        awesm::get_pretty_name<pretty_name_ns::has_get_pretty_name>() ==
        std::string_view{"custom_name"}
    );

    REQUIRE
    (
        awesm::get_pretty_name<pretty_name_ns::state>() ==
        std::string_view{"my_state"}
    );

    REQUIRE
    (
        awesm::get_pretty_name<pretty_name_ns::subsm>() ==
        std::string_view{"my_subsm"}
    );
}
