//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::region_path class template
*/

#ifndef MAKI_REGION_PATH_HPP
#define MAKI_REGION_PATH_HPP

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
instance of the @ref region_path_element struct template. These instances are
sequenced in the template argument list given to the @ref region_path
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
using region_path_t = maki::region_path
<
    maki::region_path_element<machine_def, 0>,
    maki::region_path_element<some_submachine, 1>
>;
```

This is admittedly verbose and inconvenient. This is why the library provides
@ref region_path_c and @ref region_path.add to make things much more terse:
```cpp
//Path to the exam same region
using same_region_path_t = maki::region_path_c<machine_def>::add<some_submachine, 1>;
```

@{
*/

template<class... Elems>
class region_path;

namespace detail
{
    template<class... Elems>
    constexpr auto make_region_path(const Elems&... elems)
    {
        return region_path<Elems...>{elems...};
    }

    template<class Elem>
    struct to_region_path_storage_type
    {
        using type = const std::decay_t<Elem>&;
    };

    template<>
    struct to_region_path_storage_type<int>
    {
        using type = int;
    };
}

/**
@brief Represents a path to a region.

@tparam Elems the elements, which must be instances of @ref region_path_element
*/
template<class... Elems>
class region_path
{
public:
    constexpr region_path(const Elems&... elems):
        elems_(detail::distributed_construct, elems...)
    {
    }

    constexpr region_path(const region_path& other) = default;

    constexpr region_path(region_path&& other) noexcept = default;

    constexpr region_path& operator=(const region_path& other) = default;

    constexpr region_path& operator=(region_path&& other) noexcept = default;

    ~region_path() = default;

    template<int Index>
    [[nodiscard]] constexpr decltype(auto) at() const
    {
        return detail::tuple_get<Index>(elems_);
    }

    [[nodiscard]] constexpr decltype(auto) front() const
    {
        return at<0>();
    }

    /**
    @brief A type alias to a @ref region_path with an appended @ref
    region_path_element.

    @tparam MachineDef see @ref region_path_element
    @tparam RegionIndex see @ref region_path_element; can be omitted if (and
    only if) `MachineDef` contains only one region
    */
    template<class MachineConf>
    [[nodiscard]] constexpr auto add(const MachineConf& mach_conf, const int region_index) const
    {
        return tuple_apply
        (
            elems_,
            [](const MachineConf& mach_conf, const int region_index, const auto&... elems)
            {
                return detail::make_region_path(elems..., mach_conf, region_index);
            },
            mach_conf,
            region_index
        );
    }

    [[nodiscard]] constexpr auto pop_front() const
    {
        return tuple_apply
        (
            elems_,
            [](const auto& /*elem*/, const auto&... elems)
            {
                return detail::make_region_path(elems...);
            }
        );
    }

private:
    detail::tuple<typename detail::to_region_path_storage_type<Elems>::type...> elems_;
};

template<class... Elems>
region_path(const Elems&...) -> region_path<Elems...>;

template<class... Ts, class... Us>
constexpr bool operator==(const region_path<Ts...> /*lhs*/, const region_path<Us...> /*rhs*/)
{
    return (std::is_same_v<Ts, Us> && ...);
}

/**
@brief A handy variable template for defining a @ref region_path with a single
@ref region_path_element.

@tparam MachineDef see @ref region_path_element
@tparam RegionIndex see @ref region_path_element; can be omitted if (and
only if) `MachineDef` contains only one region
*/
template<const auto& MachineConf, int RegionIndex>
inline constexpr auto region_path_c = region_path<>{}.add(MachineConf, RegionIndex);

namespace detail
{
    template<const auto& RegionPath, int Index>
    std::string region_path_element_to_string()
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
    struct region_path_to_string_impl;

    template<const auto& RegionPath, int... Indexes>
    struct region_path_to_string_impl<RegionPath, std::integer_sequence<int, Indexes...>>
    {
        static constexpr auto call()
        {
            return (region_path_element_to_string<RegionPath, Indexes>() + ...);
        }
    };
}

template<class RegionPathConstant>
auto region_path_to_string(const RegionPathConstant& /*path*/)
{
    constexpr auto size = detail::tlu::size_v<std::decay_t<decltype(RegionPathConstant::value)>>;
    using idx_sequence_t = std::make_integer_sequence<int, size>;
    return detail::region_path_to_string_impl<RegionPathConstant::value, idx_sequence_t>::call();
}

/**
@}
*/

} //namespace

#endif
