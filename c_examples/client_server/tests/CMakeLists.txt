enable_testing()

add_executable(test_client test_client.c)
add_executable(test_server test_server.c)
add_executable(test_utils test_utils.c)

target_link_libraries(test_client client utils)
target_link_libraries(test_server server utils)
target_link_libraries(test_utils utils)

add_test(NAME test_client COMMAND test_client)
add_test(NAME test_server COMMAND test_server)
add_test(NAME test_utils COMMAND test_utils)
