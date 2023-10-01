//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace pretty_name_ns
{
    struct test{};

    template<class T, class U>
    class templ{};

    struct state
    {
        static constexpr auto conf = maki::state_conf_c
            .enable_pretty_name()
        ;

        static const char* pretty_name()
        {
            return "my_state";
        }
    };

    using submachine_transition_table = maki::transition_table
        ::add<state, maki::null, maki::null>
    ;

    struct submachine
    {
        static constexpr auto conf = maki::submachine_conf_c
            .set_transition_tables<submachine_transition_table>()
            .enable_pretty_name()
        ;

        static const char* pretty_name()
        {
            return "my_submachine";
        }
    };

    struct context
    {
    };

    using transition_table_t = maki::transition_table
        ::add<state, maki::null, maki::null>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::submachine_conf_c
            .set_transition_tables<transition_table_t>()
            .set_context_type<context>()
            .enable_pretty_name()
        ;

        static const char* pretty_name()
        {
            return "my_sm";
        }
    };

    using machine_t = maki::machine<machine_def>;

    struct region_path{};
}

TEST_CASE("pretty_name")
{
    REQUIRE
    (
        maki::detail::decayed_type_name<pretty_name_ns::test>() ==
        std::string_view{"test"}
    );

    REQUIRE
    (
        maki::detail::decayed_type_name<pretty_name_ns::templ<int, pretty_name_ns::test>>() ==
        std::string_view{"templ"}
    );

    REQUIRE
    (
        maki::pretty_name<pretty_name_ns::state>() ==
        std::string_view{"my_state"}
    );

    REQUIRE
    (
        maki::pretty_name<pretty_name_ns::submachine>() ==
        std::string_view{"my_submachine"}
    );
}
