#Copyright Florian Goujeon 2021 - 2024.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

cmake_minimum_required(VERSION 3.10)

project(maki)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

option(MAKI_BUILD_EXAMPLES "Build example executables" OFF)
option(MAKI_BUILD_TESTS "Build test executable" OFF)
option(MAKI_FORCE_CATCH2_V2 "Force version 2 of catch2" OFF)

add_library(maki INTERFACE)
target_include_directories(maki INTERFACE include)

if(MAKI_BUILD_TESTS)
    find_package(Catch2 REQUIRED)
endif()

if(MAKI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(MAKI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
