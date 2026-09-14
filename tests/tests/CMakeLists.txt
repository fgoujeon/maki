#Copyright Florian Goujeon 2021 - 2026.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

set(TARGET maki-test)

file(GLOB_RECURSE CPP_FILES *.cpp)
file(GLOB_RECURSE AOS_FILES *.aos.hpp)

# Generate `.cpp` files for async-or-sync test cases
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
unset(GENERATED_FILES)
foreach(AOS_FILE ${AOS_FILES})
    file(RELATIVE_PATH AOS_FILE_REL "${CMAKE_CURRENT_LIST_DIR}/src" "${AOS_FILE}")
    #message(STATUS ${AOS_FILE_REL})

    # Use an intermediate file to avoid overwriting identical content and cause
    # a useless rebuild.
    set(TEMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/generated-temp/temp.cpp")

    set(GENERATED_SYNC_CPP_FILE "${GENERATED_DIR}/${AOS_FILE_REL}.cpp")
    file(
        WRITE
        "${TEMP_FILE}"
        "#include <src/aos/sync.hpp>\n#include <src/${AOS_FILE_REL}>")
    file(COPY_FILE "${TEMP_FILE}" "${GENERATED_SYNC_CPP_FILE}" ONLY_IF_DIFFERENT)
    list(APPEND GENERATED_FILES "${GENERATED_SYNC_CPP_FILE}")

    set(GENERATED_ASYNC_CPP_FILE "${GENERATED_DIR}/async_${AOS_FILE_REL}.cpp")
    file(
        WRITE
        "${TEMP_FILE}"
        "#include <src/aos/async.hpp>\n#include <src/${AOS_FILE_REL}>")
    file(COPY_FILE "${TEMP_FILE}" "${GENERATED_ASYNC_CPP_FILE}" ONLY_IF_DIFFERENT)
    list(APPEND GENERATED_FILES "${GENERATED_ASYNC_CPP_FILE}")
endforeach()

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${CPP_FILES})
source_group(TREE ${GENERATED_DIR} FILES ${GENERATED_FILES})
add_executable(${TARGET} ${CPP_FILES} ${GENERATED_FILES})

target_include_directories(
    ${TARGET}
    PRIVATE
        . # To allow `#include <src/...>
)

target_link_libraries(
    ${TARGET}
    PRIVATE
        maki
)

if(MAKI_BUILD_TESTS_20)
    target_compile_definitions(
        ${TARGET}
        PRIVATE
            MAKI_BUILD_TESTS_20
    )
    target_link_libraries(
        ${TARGET}
        PRIVATE
            Boost::cobalt
    )
endif()

if(TARGET Catch2::Catch2WithMain AND NOT MAKI_FORCE_CATCH2_V2) #v3
    target_compile_definitions(
        ${TARGET}
        PRIVATE
            MAKI_CATCH2_VERSION=3
    )
    target_link_libraries(
        ${TARGET}
        PRIVATE
            Catch2::Catch2WithMain
    )
else() #v2
    target_compile_definitions(
        ${TARGET}
        PRIVATE
            MAKI_CATCH2_VERSION=2
    )
    target_link_libraries(
        ${TARGET}
        PRIVATE
            Catch2::Catch2
    )
endif()

add_test(
    NAME ${TARGET}
    COMMAND ${TARGET}
)
