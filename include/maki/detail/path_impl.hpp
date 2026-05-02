//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_PATH_IMPL_HPP
#define MAKI_DETAIL_PATH_IMPL_HPP

#include "ipath_util.hpp"
#include "index_sequence.hpp"
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

    template<const auto& MachineConf>
    struct to_string_left_fold_operation
    {
        template
        <
            int Index,
            class PreviousResultElemIpath
        >
        static constexpr auto call
        (
            const to_string_left_fold_result<PreviousResultElemIpath>& previous_result
        )
        {
            using current_elem_ipath_t = index_sequence_push_back_t
            <
                PreviousResultElemIpath,
                Index
            >;

            constexpr auto is_transition_table_elem =
                index_sequence_size_v<current_elem_ipath_t> % 2 == 1
            ;

            if constexpr(is_transition_table_elem)
            {
                const auto str = previous_result.str + std::to_string(Index) + "/";
                return to_string_left_fold_result<current_elem_ipath_t>{str};
            }
            else
            {
                constexpr const auto& stt_mold = ipath_to_state_mold_v<MachineConf, current_elem_ipath_t>;
                const auto state_pretty_name = detail::pretty_name<stt_mold>();
                const auto str = previous_result.str + std::string{state_pretty_name} + "/";
                return to_string_left_fold_result<current_elem_ipath_t>{str};
            }
        }
    };
}

template<const auto& MachineConf, class Ipath>
class path_impl
{
public:
    constexpr path_impl() = default;

    [[nodiscard]] std::string to_string() const
    {
        auto str =
            index_sequence_left_fold
            <
                Ipath,
                path_impl_detail::to_string_left_fold_operation<MachineConf>
            >
            (
                path_impl_detail::to_string_left_fold_result<index_sequence<>>{""}
            ).str
        ;
        str.resize(str.size() - 1); //Remove last separator
        return str;
    }
};

} //namespace

#endif
