#Copyright Florian Goujeon 2021.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/fgfsm

cmake_minimum_required(VERSION 3.10)

set(TARGET test)

file(GLOB_RECURSE SOURCE_FILES *)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${SOURCE_FILES})
add_executable(${TARGET} ${SOURCE_FILES})

set_target_properties(
    ${TARGET} PROPERTIES
    CXX_STANDARD 17
)

if(FGFSM_ENABLE_BENCHMARKING)
    target_compile_definitions(
        ${TARGET}
        PRIVATE
            CATCH_CONFIG_ENABLE_BENCHMARKING
    )
endif()

target_link_libraries(
    ${TARGET}
    PRIVATE
        fgfsm
        Catch2::Catch2
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(
        ${TARGET}
        PRIVATE
            -Wall -Werror -pedantic
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(
        ${TARGET}
        PRIVATE
            /W4 /WX /permissive-
    )
endif()

add_test(
    NAME ${TARGET}
    COMMAND ${TARGET}
)
