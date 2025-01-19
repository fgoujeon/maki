//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

struct global_namespace_struct{};

constexpr global_namespace_struct global_namespace_constant;

constexpr auto default_state_from_global_namespace = maki::state_conf{};

namespace pretty_name_ns
{
    struct test{};

    constexpr test test_constant;

    template<class T, class U>
    struct templ
    {
        static constexpr auto inner_state = maki::state_conf{};

        template<class T2>
        static constexpr auto inner_state_templ = maki::state_conf{};
    };

    template<class T>
    class templ_inner{};

    constexpr auto state = maki::state_conf{}
        .pretty_name("my_state")
    ;

    constexpr auto default_state = maki::state_conf{};

    constexpr auto composite_state_transition_table = maki::transition_table{}
        (state, maki::null, maki::null)
    ;

    constexpr auto composite_state = maki::state_conf{}
        .transition_tables(composite_state_transition_table)
        .pretty_name("my_composite_state")
    ;

    struct context
    {
    };

    constexpr auto transition_table = maki::transition_table{}
        (state, maki::null, maki::null)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("pretty_name")
{
    CHECK
    (
        maki::detail::decayed_type_name<global_namespace_struct>() ==
        std::string_view{"global_namespace_struct"}
    );

    CHECK
    (
        maki::detail::decayed_constant_name<global_namespace_constant>() ==
        std::string_view{"global_namespace_constant"}
    );

    CHECK
    (
        maki::detail::decayed_type_name<pretty_name_ns::test>() ==
        std::string_view{"test"}
    );

    CHECK
    (
        maki::detail::decayed_constant_name<pretty_name_ns::test_constant>() ==
        std::string_view{"test_constant"}
    );

    CHECK
    (
        maki::detail::decayed_type_name<pretty_name_ns::templ<int, pretty_name_ns::test>>() ==
        std::string_view{"templ"}
    );

    CHECK
    (
        maki::detail::decayed_type_name<pretty_name_ns::templ<pretty_name_ns::templ_inner<int>, pretty_name_ns::templ_inner<char>>>() ==
        std::string_view{"templ"}
    );

    CHECK
    (
        maki::detail::pretty_name<default_state_from_global_namespace>() ==
        std::string_view{"default_state_from_global_namespace"}
    );

    CHECK
    (
        maki::detail::pretty_name<pretty_name_ns::state>() ==
        std::string_view{"my_state"}
    );

    CHECK
    (
        maki::detail::pretty_name<pretty_name_ns::templ<int, double>::inner_state>() ==
        std::string_view{"inner_state"}
    );

    CHECK
    (
        maki::detail::pretty_name<pretty_name_ns::templ<int, std::string_view>::inner_state_templ<std::string_view>>() ==
        std::string_view{"inner_state_templ"}
    );

    CHECK
    (
        maki::detail::pretty_name<pretty_name_ns::default_state>() ==
        std::string_view{"default_state"}
    );

    CHECK
    (
        maki::detail::pretty_name<pretty_name_ns::composite_state>() ==
        std::string_view{"my_composite_state"}
    );
}
