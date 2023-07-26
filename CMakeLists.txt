cmake_minimum_required(VERSION 3.16)
project(villagepp)

set(CMAKE_CXX_STANDARD 17)

set(PROJECT_SOURCE_DIR src)

add_subdirectory(src)

file(GLOB SOURCES src/*.cpp)
add_executable(Main ${SOURCES})

target_link_libraries(Main PUBLIC libmcpp.a)