#Copyright Florian Goujeon 2021.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/fgfsm

cmake_minimum_required(VERSION 3.10)

project(fgfsm)

find_package(Catch2 2.13 REQUIRED)

add_subdirectory(fgfsm)
add_subdirectory(test)
