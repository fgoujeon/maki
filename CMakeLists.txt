#Copyright Florian Goujeon 2021.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/fgfsm

cmake_minimum_required(VERSION 3.10)

project(fgfsm)

option(FGFSM_BUILD_TEST "Build test executable" OFF)
option(FGFSM_BUILD_EXAMPLES "Build examples" OFF)
option(FGFSM_ENABLE_BENCHMARKING "Enable benchmarking in test executable" OFF)

add_subdirectory(fgfsm)

if(FGFSM_BUILD_TEST)
    find_package(Catch2 2.13 REQUIRED)
    add_subdirectory(test)
endif()

if(FGFSM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
