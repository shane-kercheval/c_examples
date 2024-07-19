# Function to add test executables with debug symbols and no optimizations
function(add_test_executable TEST_NAME)
  add_executable(${TEST_NAME} ${ARGN})
  # disable optimizations; enable debug symbols
  # this is needed so we don't optimize out the test code (e.g. assert statements)
  target_compile_options(${TEST_NAME} PRIVATE -O0 -g)
  target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endfunction()

# Add your test executables
add_test_executable(test_utils test_utils.c)
target_link_libraries(test_utils PRIVATE utils)

add_test_executable(test_buffer test_buffer.c)
target_link_libraries(test_buffer PRIVATE buffer utils)

add_test_executable(test_simulation test_simulation.c)
target_link_libraries(test_simulation PRIVATE simulation)


# If you have more tests, add them here
# For example, a test that doesn't need the utils library:
# add_test_executable(test_no_deps test_no_deps.c)

# Or a test that needs utils and another library:
# add_test_executable(test_multiple test_multiple.c)
# target_link_libraries(test_multiple PRIVATE utils other_lib)
