#Copyright Florian Goujeon 2021 - 2022.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/fgfsm

cmake_minimum_required(VERSION 3.10)

project(fgfsm)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

find_package(Catch2 2.10 REQUIRED)

option(FGFSM_BUILD_EXAMPLES "Build example executables" OFF)
option(FGFSM_BUILD_TEST "Build test executable" OFF)
option(FGFSM_ENABLE_BENCHMARKING "Enable benchmarking in test executable" OFF)

add_subdirectory(fgfsm)

if(FGFSM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(FGFSM_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
