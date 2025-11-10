#Copyright Florian Goujeon 2021 - 2025.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

# @GENERATOR_COMMENT@

cmake_minimum_required(VERSION 3.23)

project(maki VERSION @VERSION_MAJOR@.@VERSION_MINOR@.@VERSION_PATCH@ LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

option(MAKI_BUILD_EXAMPLES "Build example executables" OFF)
option(MAKI_BUILD_TESTS "Build test executable" OFF)
option(MAKI_FORCE_CATCH2_V2 "Force version 2 of catch2" OFF)
option(MAKI_INSTALL "Install the library" ON)

add_library(maki INTERFACE)

target_sources(maki
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES @INDENTED_INCLUDE_FILES@)

target_compile_features(maki INTERFACE cxx_std_17)

if(MAKI_BUILD_TESTS)
    find_package(Catch2 REQUIRED)

    enable_testing()
    add_subdirectory(tests)
endif()

if(MAKI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(MAKI_INSTALL)
    install(
        EXPORT maki_export
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/maki
        NAMESPACE maki::
        FILE maki-targets.cmake)

    install(
        TARGETS maki
        EXPORT maki_export
        FILE_SET HEADERS)

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/maki-config.cmake.in
        ${PROJECT_BINARY_DIR}/maki-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/maki
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    write_basic_package_version_file(
        ${PROJECT_BINARY_DIR}/maki-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY ExactVersion)

    install(
        FILES
            ${PROJECT_BINARY_DIR}/maki-config.cmake
            ${PROJECT_BINARY_DIR}/maki-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/maki)
endif()
