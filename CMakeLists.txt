cmake_minimum_required(VERSION 3.18)
project(VisualSort)

find_package(SDL2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_executable(VisualSort main.cpp)

target_compile_options(VisualSort PUBLIC -Wall -Wextra -Wpedantic)
target_link_libraries(VisualSort SDL2)