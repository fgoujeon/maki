#Copyright Florian Goujeon 2021 - 2022.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/fgfsm

cmake_minimum_required(VERSION 3.10)

include(fgfsm)

set(TARGET rgb_lamp)

file(GLOB_RECURSE SOURCE_FILES *)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${SOURCE_FILES})
add_executable(${TARGET} ${SOURCE_FILES})

fgfsm_target_common_options(${TARGET})

target_compile_definitions(
    ${TARGET}
    PRIVATE
        TESTING=1
)

target_link_libraries(
    ${TARGET}
    PRIVATE
        fgfsm
)
