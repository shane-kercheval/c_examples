cmake_minimum_required(VERSION 3.10)
project(ProducerConsumer VERSION 1.0 DESCRIPTION "A producer-consumer example project")

include(CTest)  # include CTest to enable unit testing found in ./tests

add_compile_options(-Wall)  # enable most warning messages from the compiler

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS FALSE)

add_subdirectory(src)
add_subdirectory(tests)

add_executable(producer_consumer src/main.c)
target_link_libraries(producer_consumer PRIVATE utils)
