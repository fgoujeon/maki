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

@tparam Sm an @ref sm or subsm definition
@tparam RegionIndex the index of the region among the regions of @ref Sm
*/
template<class Sm, int RegionIndex>
struct region_path_element
{
    static_assert(RegionIndex >= 0);

    using sm_type = Sm;
    static constexpr auto region_index = RegionIndex;

    static std::string to_string()
    {
        using transition_table_list_type = detail::option_t
        <
            typename sm_type::conf,
            detail::option_id::transition_tables
        >;

        auto str = std::string{};
        str += awesm::get_pretty_name<sm_type>();

        if constexpr(detail::tlu::size_v<transition_table_list_type> > 1)
        {
            str += "[";
            str += std::to_string(region_index);
            str += "]";
        }

        return str;
    }
};

template<class... Ts>
struct region_path_tpl;

namespace detail
{
    template<class RegionPath, class Sm, int RegionIndex>
    struct region_path_add;

    template<class... Ts, class Sm, int RegionIndex>
    struct region_path_add<region_path_tpl<Ts...>, Sm, RegionIndex>
    {
        using type = region_path_tpl<Ts..., region_path_element<Sm, RegionIndex>>;
    };

    template<class... Ts, class Sm>
    struct region_path_add<region_path_tpl<Ts...>, Sm, -1>
    {
        using conf_type = typename Sm::conf;
        using transition_table_list_type = option_t<conf_type, option_id::transition_tables>;
        static_assert
        (
            tlu::size_v<transition_table_list_type> == 1,
            "RegionIndex must be specified for multiple-region SMs"
        );

        using type = region_path_tpl<Ts..., region_path_element<Sm, 0>>;
    };
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

    @tparam Sm see @ref region_path_element
    @tparam RegionIndex see @ref region_path_element; can be omitted if (and
    only if) `Sm` contains only one region
    */
    template<class Sm, int RegionIndex = -1>
    using add = typename detail::region_path_add<region_path_tpl, Sm, RegionIndex>::type;

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
    static std::string to_string()
    {
        if constexpr(sizeof...(Ts) == 0)
        {
            return "";
        }
        else
        {
            auto str = ((Ts::to_string() + ".") + ...);
            str.resize(str.size() - 1);
            return str;
        }
    }
};

/**
@brief A handy type alias for defining a @ref region_path_tpl with a single @ref
region_path_element.

@tparam Sm see @ref region_path_element
@tparam RegionIndex see @ref region_path_element; can be omitted if (and
only if) `Sm` contains only one region
*/
template<class Sm, int RegionIndex = -1>
using region_path = region_path_tpl<>::add<Sm, RegionIndex>;

/**
@}
*/

} //namespace

#endif
