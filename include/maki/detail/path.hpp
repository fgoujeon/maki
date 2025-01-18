//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_PATH_HPP
#define MAKI_DETAIL_PATH_HPP

#include "tuple.hpp"
#include "constant.hpp"
#include "../pretty_name.hpp"
#include <string_view>
#include <string>

namespace maki::detail
{

class path_element_conf
{
public:
    template<class ConfConstant>
    constexpr path_element_conf(const ConfConstant /*conf_cst*/):
        pretty_name_fn_
        (
            []
            {
                return maki::pretty_name<ConfConstant::value>();
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
class path
{
public:
    constexpr path() = default;

    template<class ParentPath, class Elem>
    constexpr path(const ParentPath& parent_path, const Elem& elem):
        elems_(tuple_append(parent_path.elems(), elem))
    {
    }

    constexpr auto add_region_index(const int region_index) const
    {
        return path<Elems..., path_element_index>{*this, path_element_index{region_index}};
    }

    template<const auto& Conf>
    constexpr auto add_state() const
    {
        return path<Elems..., path_element_conf>{*this, path_element_conf{cref_constant<Conf>}};
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
