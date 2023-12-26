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

/**
@defgroup RegionPath Region Path

@brief A path to a region or subregion of an @ref machine.

In some places in the API, you must provide or are provided a path to a region.

Conceptually, a path to a region consists in a sequence of
`(machine_def, region_index)` pairs, starting from a region of the root @ref machine.
This is the same concept as a path to a directory on a filesystem, which
consists in a sequence of directories, starting from the root directory.

In terms of C++ code, a `(machine_def, region_index)` pair is defined with an
instance of the @ref path_element struct template. These instances are
sequenced in the template argument list given to the @ref path
template instance.
Here is an example:
```cpp
//A submachine with multiple regions, used machine_def
struct some_submachine
{
    //...
};

//A machine with a single region
struct machine_def
{
    //...
};

using machine_t = maki::machine<machine_def>;

//Path to the second region (index 1) of some_submachine
using path_t = maki::path
<
    maki::path_element<machine_def, 0>,
    maki::path_element<some_submachine, 1>
>;
```

This is admittedly verbose and inconvenient. This is why the library provides
@ref path_c and @ref path.add to make things much more terse:
```cpp
//Path to the exam same region
using same_path_t = maki::path_c<machine_def>::add<some_submachine, 1>;
```

@{
*/

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
        using type = const std::decay_t<Elem>&;
    };

    template<>
    struct to_path_storage_type<int>
    {
        using type = int;
    };
}

/**
@brief Represents a path to a region.

@tparam Elems the elements, which must be instances of @ref path_element
*/
template<class... Elems>
class path
{
public:
    constexpr path() = default;

    template<class Elem>
    constexpr explicit path(const Elem& elem):
        elems_(detail::distributed_construct, elem)
    {
    }

    constexpr explicit path(const detail::path_direct_construct_t /*tag*/, const Elems&... elems):
        elems_(detail::distributed_construct, elems...)
    {
    }

    constexpr path(const path& other) = default;

    constexpr path(path&& other) noexcept = default;

    constexpr path& operator=(const path& other) = default;

    constexpr path& operator=(path&& other) noexcept = default;

    ~path() = default;

    template<int Index>
    [[nodiscard]] constexpr decltype(auto) at() const
    {
        return detail::tuple_get<Index>(elems_);
    }

    [[nodiscard]] constexpr decltype(auto) head() const
    {
        return at<0>();
    }

    [[nodiscard]] constexpr auto tail() const
    {
        return tuple_apply
        (
            elems_,
            [](const auto& /*elem*/, const auto&... elems)
            {
                return detail::make_path(elems...);
            }
        );
    }

    template<class Elem>
    [[nodiscard]] constexpr auto operator/(const Elem& elem) const
    {
        return tuple_apply
        (
            elems_,
            [](const Elem& elem, const auto&... elems)
            {
                return detail::make_path(elems..., elem);
            },
            elem
        );
    }

private:
    detail::tuple<typename detail::to_path_storage_type<Elems>::type...> elems_;
};

path() -> path<>;

template<class Elem>
path(const Elem&) -> path<Elem>;

template<class... Elems>
path(detail::path_direct_construct_t, const Elems&...) -> path<Elems...>;

namespace detail
{
    template<const auto& RegionPath, int Index>
    std::string path_element_to_string()
    {
        constexpr decltype(auto) elem = RegionPath.template at<Index>();
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
                return std::string{maki::pretty_name<elem>()};
            }
            else
            {
                return '/' + std::string{maki::pretty_name<elem>()};
            }
        }
    }

    template<const auto& RegionPath, class IndexSequence>
    struct path_to_string_impl;

    template<const auto& RegionPath, int... Indexes>
    struct path_to_string_impl<RegionPath, std::integer_sequence<int, Indexes...>>
    {
        static constexpr auto call()
        {
            return (path_element_to_string<RegionPath, Indexes>() + ...);
        }
    };
}

template<class RegionPathConstant>
auto path_to_string(const RegionPathConstant& /*path*/)
{
    constexpr auto size = detail::tlu::size_v<std::decay_t<decltype(RegionPathConstant::value)>>;
    using idx_sequence_t = std::make_integer_sequence<int, size>;
    return detail::path_to_string_impl<RegionPathConstant::value, idx_sequence_t>::call();
}

/**
@}
*/

} //namespace

#endif
