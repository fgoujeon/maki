//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct sm_def;
    using sm_t = awesm::sm<sm_def>;

    struct context
    {
        std::string output;
    };

    namespace events
    {
        struct s0_to_s1_request{};
        struct s1_to_s2_request{};
        struct s2_to_s0_request{};
    }

    namespace states
    {
        struct s0
        {
            using conf_type = awesm::state_conf
            <
                awesm::state_options::on_entry_any,
                awesm::state_options::on_exit_any
            >;

            void on_entry()
            {
                ctx.output += "s0::on_entry;";
            }

            void on_exit()
            {
                ctx.output += "s0::on_exit;";
            }

            context& ctx;
        };

        struct s1
        {
            using conf_type = awesm::state_conf
            <
                awesm::state_options::on_entry_any,
                awesm::state_options::on_exit_any
            >;

            void on_entry()
            {
                ctx.output += "s1::on_entry;";
            }

            void on_exit()
            {
                ctx.output += "s1::on_exit;";
            }

            context& ctx;
        };

        struct s2
        {
            using conf_type = awesm::state_conf
            <
                awesm::state_options::on_entry_any,
                awesm::state_options::on_exit_any
            >;

            void on_entry()
            {
                ctx.output += "s2::on_entry;";
            }

            void on_exit()
            {
                ctx.output += "s2::on_exit;";
            }

            context& ctx;
        };
    }

    namespace actions
    {
        constexpr auto s0_to_s1 = []
        (
            awesm::sm_ref<events::s1_to_s2_request> mach,
            context& /*ctx*/,
            const auto& /*event*/
        )
        {
            mach.process_event(events::s1_to_s2_request{});
        };

        constexpr auto s1_to_s2 = []
        (
            awesm::sm_ref<events::s2_to_s0_request> mach,
            context& /*ctx*/,
            const auto& /*event*/
        )
        {
            mach.process_event(events::s2_to_s0_request{});
        };
    }

    using sm_transition_table = awesm::transition_table
    <
        awesm::row<states::s0, events::s0_to_s1_request, states::s1, actions::s0_to_s1>,
        awesm::row<states::s1, events::s1_to_s2_request, states::s2, actions::s1_to_s2>,
        awesm::row<states::s2, events::s2_to_s0_request, states::s0>
    >;

    struct sm_def
    {
        using conf_type = awesm::sm_conf<sm_transition_table, context>;
    };
}

TEST_CASE("recursive process_event")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();
    REQUIRE(ctx.output == "s0::on_entry;");

    //Indirectly process s1_to_s2_request and s2_to_s0_request
    ctx.output.clear();
    sm.process_event(events::s0_to_s1_request{});
    REQUIRE
    (
        ctx.output ==
            "s0::on_exit;"
            "s1::on_entry;"
            "s1::on_exit;"
            "s2::on_entry;"
            "s2::on_exit;"
            "s0::on_entry;"
    );
}
