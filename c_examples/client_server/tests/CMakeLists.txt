enable_testing()

add_library(unity STATIC unity/unity.c unity/unity.h)

add_executable(test_utils test_utils.c)
target_link_libraries(test_utils utils unity)
target_include_directories(test_utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/unity)
add_test(NAME test_utils COMMAND test_utils)

add_executable(test_protocol test_protocol.c)
target_link_libraries(test_protocol protocol unity)
target_include_directories(test_protocol PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/unity)
add_test(NAME test_protocol COMMAND test_protocol)
