cmake_minimum_required(VERSION 3.25)
project (geometry)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_options(-g -Wall -Wextra -Wpedantic -Wl,--stack,16777216)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(math_test math_test.cc math.cc)
target_link_libraries(math_test gtest gtest_main)


add_executable(geometry_test geometry_test.cc math.cc geometry.cc)
target_link_libraries(geometry_test gtest gtest_main)
