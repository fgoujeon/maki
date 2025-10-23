#Copyright Florian Goujeon 2021 - 2025.
#Distributed under the Boost Software License, Version 1.0.
#(See accompanying file LICENSE or copy at
#https://www.boost.org/LICENSE_1_0.txt)
#Official repository: https://github.com/fgoujeon/maki

function(maki_target_common_options TARGET)
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    target_compile_options(${TARGET} PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wsign-conversion -pedantic -Werror>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wsign-conversion -pedantic -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /WX>)
endfunction()
