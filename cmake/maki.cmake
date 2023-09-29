#Copyright Florian Goujeon 2021 - 2023.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

cmake_minimum_required(VERSION 3.10)

function(maki_target_common_options TARGET)
    set_target_properties(
        ${TARGET} PROPERTIES
        CXX_STANDARD 17
    )

    unset(WERROR)
    if(MAKI_WARNINGS_AS_ERRORS)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            set(WERROR "/WX")
        else()
            set(WERROR "-Werror")
        endif()
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(
            ${TARGET}
            PRIVATE
                -Wall -Wextra -Wsign-conversion -pedantic ${WERROR}
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(
            ${TARGET}
            PRIVATE
                -Wall -Wextra -Wsign-conversion -pedantic ${WERROR}
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(
            ${TARGET}
            PRIVATE
                /W4 /permissive- ${WERROR}
        )
    endif()
endfunction()
