cmake_minimum_required(VERSION 3.10)
project(ClientServer VERSION 1.0 DESCRIPTION "A Client-Server example project.")

include(CTest)  # include CTest to enable unit testing found in ./tests

add_compile_options(-Wall)  # enable most warning messages from the compiler

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS FALSE)

include_directories(include)

add_subdirectory(src)
add_subdirectory(tests)
