#Copyright Florian Goujeon 2021 - 2023.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/awesm

cmake_minimum_required(VERSION 3.10)

include(awesm)

set(TARGET example-events)

file(GLOB_RECURSE SOURCE_FILES *)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${SOURCE_FILES})
add_executable(${TARGET} ${SOURCE_FILES})

awesm_target_common_options(${TARGET})

target_link_libraries(
    ${TARGET}
    PRIVATE
        awesm
)
