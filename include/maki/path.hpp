//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_PATH_HPP
#define MAKI_PATH_HPP

#include "detail/friendly_impl.hpp"
#include <string>

namespace maki
{

/**
@brief Encodes a path to a state or region.
@tparam Impl implementation detail

Objects of this type can only be created by Maki itself.
*/
template<class Impl>
class path
{
public:
#ifndef MAKI_DETAIL_DOXYGEN
    constexpr path(const Impl& impl):
        impl_(impl)
    {
    }
#endif

    /**
    @brief Returns a textual representation of the path.

    For example the string `"0/my_state/1"` represents the path to the second
    region (index `1`) of a composite state whose
    @ref maki::state::pretty_name "pretty name" is `"my_state"` and that is part
    of the first region (index `0`) of a state machine.
    */
    [[nodiscard]] std::string to_string() const
    {
        return impl_.to_string();
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    using impl_type = Impl;

    impl_type impl_;
};

} //namespace

#endif
