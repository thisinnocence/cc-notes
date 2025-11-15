cmake_minimum_required(VERSION 3.10)
project(gtest_demo)

# for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Add library from src
add_library(math_lib src/add.cpp)
target_include_directories(math_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Add test executable
add_executable(tests tests/test.cpp)
target_link_libraries(tests math_lib gtest)
