#Copyright Florian Goujeon 2021 - 2024.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

cmake_minimum_required(VERSION 3.10)

add_subdirectory(actions)
add_subdirectory(events)
add_subdirectory(fan)
add_subdirectory(lamp)
add_subdirectory(rgb-lamp)

add_subdirectory(doc-concepts-context)
add_subdirectory(doc-concepts-state)
