//Copyright Florian Goujeon 2021 - 2022.
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
            awesm::state_options::get_pretty_name
        >;

        static const char* get_pretty_name()
        {
            return "my_state";
        }
    };

    auto composite_state_transition_table()
    {
        return awesm::transition_table
        <
            awesm::row<state, awesm::null, awesm::null>
        >;
    }

    struct composite_state
    {
        using conf = awesm::composite_state_conf
        <
            composite_state_transition_table,
            awesm::composite_state_options::get_pretty_name
        >;

        static const char* get_pretty_name()
        {
            return "my_composite_state";
        }
    };

    struct context
    {
    };

    auto sm_transition_table()
    {
        return awesm::transition_table
        <
            awesm::row<state, awesm::null, awesm::null>
        >;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            sm_transition_table,
            context,
            awesm::sm_options::get_pretty_name
        >;

        static const char* get_pretty_name()
        {
            return "my_sm";
        }
    };

    using sm_t = awesm::sm<sm_def>;

    struct region_path{};

    using wrapped_composite_state = awesm::detail::composite_state_wrapper
    <
        awesm::make_region_path<sm_t, 0>,
        composite_state
    >;
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
        awesm::get_pretty_name<pretty_name_ns::composite_state>() ==
        std::string_view{"my_composite_state"}
    );

    REQUIRE
    (
        awesm::get_pretty_name<pretty_name_ns::wrapped_composite_state>() ==
        std::string_view{"my_composite_state"}
    );

    REQUIRE
    (
        awesm::get_pretty_name<pretty_name_ns::sm_t>() ==
        std::string_view{"my_sm"}
    );
}
