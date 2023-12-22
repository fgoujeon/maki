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

/**
@brief Represents an element of a path to a region.

@tparam MachineDef an @ref machine or submachine definition
@tparam RegionIndex the index of the region among the regions of `MachineDef`
*/
template<const auto& MachineConf, int RegionIndex>
struct region_path_element
{
    static_assert(RegionIndex >= 0);

    static constexpr const auto& machine_conf = MachineConf;
    static constexpr auto region_index = RegionIndex;
};

template<class... Ts>
struct region_path;

namespace detail
{
    template<class Element>
    struct region_path_element_add_pretty_name_holder;

    template<const auto& MachineConf, int RegionIndex>
    struct region_path_element_add_pretty_name_holder<region_path_element<MachineConf, RegionIndex>>
    {
        static void add_pretty_name(std::ostringstream& oss, bool& first)
        {
            using transition_table_list_type = decltype(MachineConf.transition_tables_);

            if(first)
            {
                first = false;
            }
            else
            {
                oss << '.';
            }

            oss << maki::pretty_name<MachineConf>();

            if constexpr(detail::tlu::size_v<transition_table_list_type> > 1)
            {
                oss << '[' << std::to_string(RegionIndex) << ']';
            }
        }
    };

    template<class... Elements>
    auto region_path_pretty_name()
    {
        if constexpr(sizeof...(Elements) == 0)
        {
            return "";
        }
        else
        {
            auto first = true;
            auto oss = std::ostringstream{};

            (
                (detail::region_path_element_add_pretty_name_holder<Elements>::add_pretty_name(oss, first)),
                ...
            );

            return oss.str();
        }
    }
}

/**
@brief Represents a path to a region.

@tparam Ts the elements, which must be instances of @ref region_path_element
*/
template<class... Ts>
struct region_path
{
    /**
    @brief A type alias to a @ref region_path with an appended @ref
    region_path_element.

    @tparam MachineDef see @ref region_path_element
    @tparam RegionIndex see @ref region_path_element; can be omitted if (and
    only if) `MachineDef` contains only one region
    */
    template<const auto& MachineConf, int RegionIndex>
    [[nodiscard]] constexpr auto add() const
    {
        return region_path<Ts..., region_path_element<MachineConf, RegionIndex>>{};
    }

    /**
    @brief Builds a textual representation of the path.

    This function builds strings such as `"root-machine[0].submachine[2].subsubmachine"`,
    where:

    - `root-machine` is the "pretty name" of an `machine` that
    defines several regions;
    - `[0]` references the first region defined by `root-machine`;
    - `submachine` is the "pretty name" of a submachine, direct
    child of `root-machine`, that defines several regions;
    - `[2]` references the third region defined by `submachine`;
    - `subsubmachine` is the "pretty name" of another submachine,
    direct child of `submachine`, that defines only one region (hence the absence
    of region index).
    */
    static std::string_view to_string()
    {
        static const auto str = detail::region_path_pretty_name<Ts...>();
        return str;
    }
};

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
inline constexpr auto region_path_c = region_path<>{}.add<MachineConf, RegionIndex>();

/**
@}
*/

} //namespace

#endif
