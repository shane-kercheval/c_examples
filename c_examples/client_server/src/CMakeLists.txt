# NOTE: A static library is a collection of object files that are linked into the executable at
# compile time, rather than at runtime, which can increase the speed of the executable.
add_library(client STATIC client.c)
add_library(server STATIC server.c)
add_library(utils STATIC utils.c)

target_link_libraries(client utils)
target_link_libraries(server utils)
