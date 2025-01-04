//Copyright Florian Goujeon 2021 - 2024.
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
*/
#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class Impl>
#endif
class path
{
public:
    [[nodiscard]] std::string to_string() const
    {
        return impl_.get().to_string();
    }

private:
    template<const auto& TransitionTable, const auto& Path>
    friend class detail::region;

    path(const Impl& impl):
        impl_(impl)
    {
    }

    std::reference_wrapper<const Impl> impl_;
};

} //namespace

#endif
