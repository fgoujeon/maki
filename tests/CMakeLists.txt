#Copyright Florian Goujeon 2021 - 2024.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

cmake_minimum_required(VERSION 3.10)

add_subdirectory(tests)
add_subdirectory(example-checker)

#Test examples
set(EXAMPLES_DIR ${CMAKE_SOURCE_DIR}/examples)
file(
    GLOB_RECURSE SUBDIRS
    LIST_DIRECTORIES true
    RELATIVE ${EXAMPLES_DIR}
    ${EXAMPLES_DIR}/*)
foreach(SUBDIR ${SUBDIRS})
    if(EXISTS ${EXAMPLES_DIR}/${SUBDIR}/CMakeLists.txt)
        set(EXAMPLE_DIR ${EXAMPLES_DIR}/${SUBDIR})
        string(REPLACE "/" "-" EXAMPLE_TARGET "example-${SUBDIR}")
        add_test(
            NAME test-${EXAMPLE_TARGET}
            COMMAND example-checker $<TARGET_FILE:${EXAMPLE_TARGET}> ${EXAMPLE_DIR}/expected-output.txt
        )
    endif()
endforeach()
