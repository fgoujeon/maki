//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_PATH_IMPL_HPP
#define MAKI_DETAIL_PATH_IMPL_HPP

#include "tuple.hpp"
#include "constant.hpp"
#include "pretty_name.hpp"
#include <string_view>
#include <string>

namespace maki::detail
{

class path_element_state
{
public:
    template<class MoldConstant>
    constexpr path_element_state(const MoldConstant /*mold_cst*/):
        pretty_name_fn_
        (
            []
            {
                return detail::pretty_name<MoldConstant::value>();
            }
        )
    {
    }

    [[nodiscard]] std::string pretty_name() const
    {
        return std::string{pretty_name_fn_()};
    }

private:
    using pretty_name_fn = std::string_view(*)();

    pretty_name_fn pretty_name_fn_ = nullptr;
};

class path_element_index
{
public:
    constexpr path_element_index(const int index):
        index_(index)
    {
    }

    [[nodiscard]] std::string pretty_name() const
    {
        return std::to_string(index_);
    }

private:
    int index_ = 0;
};

template<class... Elems>
class path_impl
{
public:
    constexpr path_impl() = default;

    template<class ParentPath, class Elem>
    constexpr path_impl(const ParentPath& parent_path, const Elem& elem):
        elems_(parent_path.elems().append(elem))
    {
    }

    constexpr auto add_region_index(const int region_index) const
    {
        return path_impl<Elems..., path_element_index>{*this, path_element_index{region_index}};
    }

    template<const auto& Mold>
    constexpr auto add_state() const
    {
        return path_impl<Elems..., path_element_state>{*this, path_element_state{cref_constant<Mold>}};
    }

    constexpr const auto& elems() const
    {
        return elems_;
    }

    [[nodiscard]] std::string to_string() const
    {
        auto str = tuple_apply
        (
            elems_,
            [](const auto&... elems)
            {
                return ((elems.pretty_name() + "/") + ...);
            }
        );
        str.resize(str.size() - 1); //Remove last separator
        return str;
    }

private:
    tuple<Elems...> elems_;
};

} //namespace

#endif
