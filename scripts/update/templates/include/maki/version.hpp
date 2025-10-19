//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

// @GENERATOR_COMMENT@

/**
@file
@brief Defines the version of the Maki library
*/

#ifndef MAKI_VERSION_HPP
#define MAKI_VERSION_HPP

namespace maki::version
{

constexpr auto major = @VERSION_MAJOR@;
constexpr auto minor = @VERSION_MINOR@;
constexpr auto patch = @VERSION_PATCH@;

} //namespace

#endif
