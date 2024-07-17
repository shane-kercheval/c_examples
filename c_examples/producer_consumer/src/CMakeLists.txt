add_library(utils utils.c utils.h)
add_library(buffer buffer.c buffer.h)
target_link_libraries(buffer PRIVATE utils)
