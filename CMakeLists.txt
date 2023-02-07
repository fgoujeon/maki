#Copyright Florian Goujeon 2021 - 2023.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/awesm

cmake_minimum_required(VERSION 3.10)

project(awesm)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

option(AWESM_BUILD_EXAMPLES "Build example executables" OFF)
option(AWESM_BUILD_TEST "Build test executable" OFF)
option(AWESM_FORCE_CATCH2_V2 "Force version 2 of catch2" OFF)

if(AWESM_BUILD_TEST)
    find_package(Catch2 REQUIRED)
endif()

add_subdirectory(awesm)

if(AWESM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(AWESM_BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif()
