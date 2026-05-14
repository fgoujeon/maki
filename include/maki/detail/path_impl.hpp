//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_PATH_IMPL_HPP
#define MAKI_DETAIL_PATH_IMPL_HPP

#include "iseq.hpp"
#include "pretty_name.hpp"
#include <string>

namespace maki::detail
{

namespace path_impl_detail
{
    template<class ElemIpath>
    struct to_string_left_fold_result
    {
        std::string str;
    };

    template<class MachineConfHolder>
    struct to_string_left_fold_operation
    {
        template
        <
            int Id,
            class PreviousResultNodeIpath
        >
        static constexpr auto call
        (
            const to_string_left_fold_result<PreviousResultNodeIpath>& previous_result
        )
        {
            using current_node_ipath_t = iseq_push_back_t
            <
                PreviousResultNodeIpath,
                Id
            >;

            constexpr auto is_transition_table_elem =
                iseq_size_v<current_node_ipath_t> % 2 == 1
            ;

            if constexpr(is_transition_table_elem)
            {
                const auto str = previous_result.str + std::to_string(Id) + "/";
                return to_string_left_fold_result<current_node_ipath_t>{str};
            }
            else
            {
                const auto state_pretty_name = detail::pretty_name<MachineConfHolder, current_node_ipath_t>();
                const auto str = previous_result.str + std::string{state_pretty_name} + "/";
                return to_string_left_fold_result<current_node_ipath_t>{str};
            }
        }
    };
}

template<class MachineConfHolder, class Ipath>
class path_impl
{
public:
    constexpr path_impl() = default;

    [[nodiscard]] std::string to_string() const
    {
        auto str =
            iseq_left_fold
            <
                Ipath,
                path_impl_detail::to_string_left_fold_operation<MachineConfHolder>
            >
            (
                path_impl_detail::to_string_left_fold_result<iseq<>>{""}
            ).str
        ;
        str.resize(str.size() - 1); //Remove last separator
        return str;
    }
};

} //namespace

#endif
