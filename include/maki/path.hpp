//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::path class template
*/

#ifndef MAKI_PATH_HPP
#define MAKI_PATH_HPP

#include "transition_table.hpp"
#include "pretty_name.hpp"
#include "detail/tuple.hpp"
#include "detail/tlu.hpp"
#include <string>
#include <sstream>

namespace maki
{

template<class... Elems>
class path;

namespace detail
{
    struct path_direct_construct_t{};
    inline constexpr auto path_direct_construct = path_direct_construct_t{};

    template<class... Elems>
    constexpr auto make_path(const Elems&... elems)
    {
        return path<Elems...>{path_direct_construct, elems...};
    }

    template<class Elem>
    struct to_path_storage_type
    {
        using type = const std::decay_t<Elem>*;
    };

    template<>
    struct to_path_storage_type<int>
    {
        using type = int;
    };

    template<class Elem>
    constexpr auto to_path_storage(const Elem& elem)
    {
        return &elem;
    }

    inline constexpr int to_path_storage(const int elem)
    {
        return elem;
    }

    template<class Elem>
    constexpr const Elem& from_path_storage(const Elem* pelem)
    {
        return *pelem;
    }

    inline constexpr int from_path_storage(const int elem)
    {
        return elem;
    }

    template<int Index, class Path>
    [[nodiscard]] constexpr auto path_raw_at(Path& pth)
    {
        return pth.template raw_at<Index>();
    }

    template<class Path>
    [[nodiscard]] constexpr auto path_raw_head(Path& pth)
    {
        return path_raw_at<0>(pth);
    }

    template<class Path>
    [[nodiscard]] constexpr auto path_tail(Path& pth)
    {
        return pth.tail();
    }
}

/**
@brief Represents a path to a machine, submachine, state or region.

@tparam Elems the type of each element of the path

In some places in the API, you must provide or are provided a path to a state or region.

Conceptually, a path consists in a sequence of machine/submachine/state conf
reference and region indexes.
This is the same concept as a path to a directory on a filesystem, which
consists in a sequence of directories, optionaly ending with a filename.
*/
template<class... Elems>
class path
{
public:
    constexpr path() = default;

    template<class Elem>
    constexpr explicit path(const Elem& elem):
        elems_(detail::distributed_construct, detail::to_path_storage(elem))
    {
    }

    constexpr path(const path& other) = default;

    constexpr path(path&& other) noexcept = default;

    constexpr path& operator=(const path& other) = default;

    constexpr path& operator=(path&& other) noexcept = default;

    ~path() = default;

    static constexpr int size()
    {
        return static_cast<int>(sizeof...(Elems));
    }

    static constexpr bool empty()
    {
        return sizeof...(Elems) == 0;
    }

    template<int Index>
    [[nodiscard]] constexpr decltype(auto) at() const
    {
        return detail::from_path_storage(detail::tuple_get<Index>(elems_));
    }

    template<class Elem>
    [[nodiscard]] constexpr auto operator/(const Elem& elem) const
    {
        return tuple_apply
        (
            elems_,
            [](const Elem& elem, const auto... elems)
            {
                return detail::make_path(detail::from_path_storage(elems)..., elem);
            },
            elem
        );
    }

private:
    template<class... Elems2>
    friend constexpr auto detail::make_path(const Elems2&...);

    template<int Index, class Path>
    friend constexpr auto detail::path_raw_at(Path& pth);

    template<class Path>
    friend constexpr auto detail::path_tail(Path& pth);

    constexpr explicit path(const detail::path_direct_construct_t /*tag*/, const Elems&... elems):
        elems_(detail::distributed_construct, detail::to_path_storage(elems)...)
    {
    }

    template<int Index>
    [[nodiscard]] constexpr auto raw_at() const
    {
        return detail::tuple_get<Index>(elems_);
    }

    [[nodiscard]] constexpr auto tail() const
    {
        return tuple_apply
        (
            elems_,
            [](const auto /*elem*/, const auto... elems)
            {
                return detail::make_path(detail::from_path_storage(elems)...);
            }
        );
    }

    detail::tuple<typename detail::to_path_storage_type<Elems>::type...> elems_;
};

path() -> path<>;

template<class Elem>
path(const Elem&) -> path<Elem>;

namespace detail
{
    template<const auto& Path, int Index>
    std::string path_element_to_string()
    {
        constexpr auto elem = path_raw_at<Index>(Path);
        using elem_type = std::decay_t<decltype(elem)>;

        if constexpr(std::is_same_v<elem_type, int>)
        {
            if constexpr(Index == 0)
            {
                return std::to_string(elem);
            }
            else
            {
                return '/' + std::to_string(elem);
            }
        }
        else
        {
            if constexpr(Index == 0)
            {
                return std::string{maki::pretty_name<*elem>()};
            }
            else
            {
                return '/' + std::string{maki::pretty_name<*elem>()};
            }
        }
    }

    template<const auto& Path, class IndexSequence>
    struct path_to_string_impl;

    template<const auto& Path, int... Indexes>
    struct path_to_string_impl<Path, std::integer_sequence<int, Indexes...>>
    {
        static constexpr auto call()
        {
            return (path_element_to_string<Path, Indexes>() + ...);
        }
    };
}

template<class... Elems, const path<Elems...>& Path>
std::string to_string(const std::integral_constant<const path<Elems...>&, Path>& /*path*/)
{
    using idx_sequence_t = std::make_integer_sequence<int, Path.size()>;
    return detail::path_to_string_impl<Path, idx_sequence_t>::call();
}

inline constexpr auto empty_path_c = path<>{};

} //namespace

#endif
