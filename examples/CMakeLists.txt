#Copyright Florian Goujeon 2021 - 2025.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE SUBDIRS LIST_DIRECTORIES true *)
foreach(SUBDIR ${SUBDIRS})
    if(EXISTS ${SUBDIR}/CMakeLists.txt)
        add_subdirectory(${SUBDIR})
    endif()
endforeach()
