add_executable(test_utils test_utils.c)
target_link_libraries(test_utils PRIVATE utils)
target_include_directories(test_utils PRIVATE ${CMAKE_SOURCE_DIR}/src)
add_test(NAME test_utils COMMAND test_utils)
