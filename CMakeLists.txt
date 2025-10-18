#Copyright Florian Goujeon 2021 - 2025.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

cmake_minimum_required(VERSION 3.23)

project(maki VERSION 1.0.2 LANGUAGES CXX)
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
    FILES
        # /
        include/maki.hpp
        include/maki/action.hpp
        include/maki/context.hpp
        include/maki/event.hpp
        include/maki/event_set.hpp
        include/maki/events.hpp
        include/maki/fin.hpp
        include/maki/guard.hpp
        include/maki/ini.hpp
        include/maki/machine.hpp
        include/maki/machine_conf.hpp
        include/maki/machine_ref.hpp
        include/maki/machine_ref_conf.hpp
        include/maki/null.hpp
        include/maki/path.hpp
        include/maki/region.hpp
        include/maki/state.hpp
        include/maki/state_mold.hpp
        include/maki/state_set.hpp
        include/maki/states.hpp
        include/maki/transition_table.hpp
        include/maki/version.hpp

        # detail/
        include/maki/detail/call.hpp
        include/maki/detail/compiler.hpp
        include/maki/detail/constant.hpp
        include/maki/detail/context_holder.hpp
        include/maki/detail/equals.hpp
        include/maki/detail/event_action.hpp
        include/maki/detail/function_queue.hpp
        include/maki/detail/impl.hpp
        include/maki/detail/integer_constant_sequence.hpp
        include/maki/detail/machine_conf_fwd.hpp
        include/maki/detail/maybe_bool_util.hpp
        include/maki/detail/mix.hpp
        include/maki/detail/noinline.hpp
        include/maki/detail/overload_priority.hpp
        include/maki/detail/path.hpp
        include/maki/detail/pretty_name.hpp
        include/maki/detail/region.hpp
        include/maki/detail/region_fwd.hpp
        include/maki/detail/set.hpp
        include/maki/detail/signature_macros.hpp
        include/maki/detail/state_id_to_type.hpp
        include/maki/detail/state_id_traits.hpp
        include/maki/detail/state_mold_fwd.hpp
        include/maki/detail/state_type_list_filters.hpp
        include/maki/detail/tlu.hpp
        include/maki/detail/transition_table_digest.hpp
        include/maki/detail/transition_table_filters.hpp
        include/maki/detail/tuple.hpp
        include/maki/detail/type.hpp
        include/maki/detail/type_list.hpp
        include/maki/detail/type_name.hpp
        include/maki/detail/type_set.hpp
        include/maki/detail/type_traits.hpp

        # detail/state_impls/
        include/maki/detail/state_impls/composite.hpp
        include/maki/detail/state_impls/composite_fwd.hpp
        include/maki/detail/state_impls/composite_no_context.hpp
        include/maki/detail/state_impls/composite_no_context_fwd.hpp
        include/maki/detail/state_impls/simple.hpp
        include/maki/detail/state_impls/simple_fwd.hpp
        include/maki/detail/state_impls/simple_no_context.hpp
        include/maki/detail/state_impls/simple_no_context_fwd.hpp

        # detail/tlu/
        include/maki/detail/tlu/apply.hpp
        include/maki/detail/tlu/back.hpp
        include/maki/detail/tlu/contains.hpp
        include/maki/detail/tlu/contains_if.hpp
        include/maki/detail/tlu/empty.hpp
        include/maki/detail/tlu/filter.hpp
        include/maki/detail/tlu/find.hpp
        include/maki/detail/tlu/find_if.hpp
        include/maki/detail/tlu/find_if_or.hpp
        include/maki/detail/tlu/for_each.hpp
        include/maki/detail/tlu/for_each_or.hpp
        include/maki/detail/tlu/front.hpp
        include/maki/detail/tlu/get.hpp
        include/maki/detail/tlu/left_fold.hpp
        include/maki/detail/tlu/pop_front.hpp
        include/maki/detail/tlu/push_back.hpp
        include/maki/detail/tlu/push_back_all_unique.hpp
        include/maki/detail/tlu/push_back_if.hpp
        include/maki/detail/tlu/push_back_unique.hpp
        include/maki/detail/tlu/push_front.hpp
        include/maki/detail/tlu/size.hpp
)
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
    install(EXPORT maki_export DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/maki NAMESPACE maki:: FILE maki-targets.cmake)
    install(TARGETS maki EXPORT maki_export FILE_SET HEADERS)

    configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/maki-config.cmake.in ${PROJECT_BINARY_DIR}/maki-config.cmake INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/maki NO_CHECK_REQUIRED_COMPONENTS_MACRO)
    write_basic_package_version_file(${PROJECT_BINARY_DIR}/maki-config-version.cmake VERSION ${PROJECT_VERSION} COMPATIBILITY ExactVersion)
    install(FILES ${PROJECT_BINARY_DIR}/maki-config.cmake ${PROJECT_BINARY_DIR}/maki-config-version.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/maki)
endif()
