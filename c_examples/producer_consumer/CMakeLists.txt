cmake_minimum_required(VERSION 3.10)
project(ProducerConsumer VERSION 1.0 DESCRIPTION "A producer-consumer example project")

include(CTest)  # include CTest to enable unit testing found in ./tests

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall)  # enable most warning messages from the compiler

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS FALSE)

add_subdirectory(src)
add_subdirectory(tests)

add_executable(producer_consumer src/main.c)
target_link_libraries(producer_consumer PRIVATE buffer utils)

# Define NDEBUG for Release builds
# NDEBUG is a macro that disables assertions in the code
target_compile_definitions(producer_consumer PRIVATE 
  $<$<CONFIG:Release>:NDEBUG>
)
# optimization flags
target_compile_options(producer_consumer PRIVATE
  $<$<CONFIG:Debug>:-O0 -g>  # disables optimization; enables debug symbols
  $<$<CONFIG:Release>:-O3>  # enables high-level optimizations
)
