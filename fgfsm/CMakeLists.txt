#Copyright Florian Goujeon 2021 - 2022.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/fgfsm

cmake_minimum_required(VERSION 3.10)

set(TARGET fgfsm)

add_library(${TARGET} INTERFACE)
target_include_directories(${TARGET} INTERFACE include)
