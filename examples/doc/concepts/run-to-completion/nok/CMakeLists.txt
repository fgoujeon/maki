#Copyright Florian Goujeon 2021 - 2024.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

cmake_minimum_required(VERSION 3.10)

include(maki)

set(TARGET example-doc-concepts-run-to-completion-nok)

file(GLOB_RECURSE SOURCE_FILES ../ok/src/main.cpp)
add_executable(${TARGET} ${SOURCE_FILES})

maki_target_common_options(${TARGET})

target_link_libraries(
    ${TARGET}
    PRIVATE
        maki
)
