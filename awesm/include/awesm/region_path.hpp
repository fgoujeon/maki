//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_REGION_PATH_HPP
#define AWESM_REGION_PATH_HPP

#include "transition_table.hpp"
#include "pretty_name.hpp"
#include "detail/tlu.hpp"
#include <string>
#include <sstream>

namespace awesm
{

/**
@defgroup RegionPath Region Path

@brief A path to a region or subregion of an @ref sm.

In some places in the API, you must provide or are provided a path to a region.

Conceptually, a path to a region consists in a sequence of
`(sm_def, region_index)` pairs, starting from a region of the root @ref sm.
This is the same concept as a path to a directory on a filesystem, which
consists in a sequence of directories, starting from the root directory.

In terms of C++ code, a `(sm_def, region_index)` pair is defined with an
instance of the @ref region_path_element struct template. These instances are
sequenced in the template argument list given to the @ref region_path_tpl
template instance.
Here is an example:
```cpp
//A subsm with multiple regions, used sm_def
struct some_subsm
{
    //...
};

//An sm with a single region
struct sm_def
{
    //...
};

using sm_t = awesm::sm<sm_def>;

//Path to the second region (index 1) of some_subsm
using region_path_t = awesm::region_path_tpl
<
    awesm::region_path_element<sm_def, 0>,
    awesm::region_path_element<some_subsm, 1>
>;
```

This is admittedly verbose and inconvenient. This is why the library provides
@ref region_path and @ref region_path_tpl.add to make things much more terse:
```cpp
//Path to the exam same region
using same_region_path_t = awesm::region_path<sm_def>::add<some_subsm, 1>;
```

@{
*/

/**
@brief Represents an element of a path to a region.

@tparam SmDef an @ref sm or subsm definition
@tparam RegionIndex the index of the region among the regions of @ref SmDef
*/
template<class SmDef, int RegionIndex>
struct region_path_element
{
    static_assert(RegionIndex >= 0);

    using sm_def_type = SmDef;
    static constexpr auto region_index = RegionIndex;

};

template<class... Ts>
struct region_path_tpl;

namespace detail
{
    template<class RegionPath, class SmDef, int RegionIndex>
    struct region_path_add;

    template<class... Ts, class SmDef, int RegionIndex>
    struct region_path_add<region_path_tpl<Ts...>, SmDef, RegionIndex>
    {
        using type = region_path_tpl<Ts..., region_path_element<SmDef, RegionIndex>>;
    };

    template<class... Ts, class SmDef>
    struct region_path_add<region_path_tpl<Ts...>, SmDef, -1>
    {
        using conf_type = typename SmDef::conf;
        using transition_table_list_type = option_t<conf_type, option_id::transition_tables>;
        static_assert
        (
            tlu::size_v<transition_table_list_type> == 1,
            "RegionIndex must be specified for multiple-region SMs"
        );

        using type = region_path_tpl<Ts..., region_path_element<SmDef, 0>>;
    };

    template<class Element>
    struct region_path_element_add_pretty_name_holder;

    template<class SmDef, int RegionIndex>
    struct region_path_element_add_pretty_name_holder<region_path_element<SmDef, RegionIndex>>
    {
        static void add_pretty_name(std::ostringstream& oss, bool& first)
        {
            using transition_table_list_type = detail::option_t
            <
                typename SmDef::conf,
                detail::option_id::transition_tables
            >;

            if(first)
            {
                first = false;
            }
            else
            {
                oss << '.';
            }

            oss << awesm::pretty_name<SmDef>();

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
struct region_path_tpl
{
    /**
    @brief A type alias to a @ref region_path_tpl with an appended @ref
    region_path_element.

    @tparam SmDef see @ref region_path_element
    @tparam RegionIndex see @ref region_path_element; can be omitted if (and
    only if) `SmDef` contains only one region
    */
    template<class SmDef, int RegionIndex = -1>
    using add = typename detail::region_path_add<region_path_tpl, SmDef, RegionIndex>::type;

    /**
    @brief Builds a textual representation of the path.

    This function builds strings such as `"root-sm[0].subsm[2].subsubsm"`,
    where:

    - `root-sm` is the @ref PrettyPrinting "pretty name" of an `sm` that
    defines several regions;
    - `[0]` references the first region defined by `root-sm`;
    - `subsm` is the @ref PrettyPrinting "pretty name" of a submachine, direct
    child of `root-sm`, that defines several regions;
    - `[2]` references the third region defined by `subsm`;
    - `subsubsm` is the @ref PrettyPrinting "pretty name" of another submachine,
    direct child of `subsm`, that defines only one region (hence the absence
    of region index).
    */
    static std::string_view to_string()
    {
        static const auto str = detail::region_path_pretty_name<Ts...>();
        return str;
    }
};

/**
@brief A handy type alias for defining a @ref region_path_tpl with a single @ref
region_path_element.

@tparam SmDef see @ref region_path_element
@tparam RegionIndex see @ref region_path_element; can be omitted if (and
only if) `SmDef` contains only one region
*/
template<class SmDef, int RegionIndex = -1>
using region_path = region_path_tpl<>::add<SmDef, RegionIndex>;

/**
@}
*/

} //namespace

#endif
