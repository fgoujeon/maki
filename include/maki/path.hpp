//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_PATH_HPP
#define MAKI_PATH_HPP

#include "detail/region_fwd.hpp" //NOLINT misc-include-cleaner
#include <string>
#include <functional>

namespace maki
{

/**
@brief Encodes a path to a state or region.

Objects of this type can only be created by Maki itself.
*/
#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class Impl>
#endif
class path
{
public:
    /**
    @brief Returns a textual representation of the path.

    For example the string `"0/my_state/1"` represents the path to the second
    region (index `1`) of a composite state whose
    @ref maki::state::pretty_name "pretty name" is `"my_state"` and that is part
    of the first region (index `0`) of a state machine.
    */
    [[nodiscard]] std::string to_string() const
    {
        return impl_.get().to_string();
    }

private:
#ifndef MAKI_DETAIL_DOXYGEN
    template<const auto& TransitionTable, const auto& Path>
    friend class detail::region;
#endif

    constexpr path(const Impl& impl):
        impl_(impl)
    {
    }

    std::reference_wrapper<const Impl> impl_;
};

} //namespace

#endif
