cmake_minimum_required (VERSION 2.6)
project(6666)
set(OBJECT "test_grpc")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -g3 -ggdb3 -gdwarf-2")

include_directories("${PROJECT_SOURCE_DIR}")

#file(GLOB SRC_FILE "*.c")
# Only use the new files
set(SRC_FILE
    client.c
    server.c
    main.c
    foo.grpc-c.c
    foo.grpc-c.service.c
)

add_executable(${OBJECT} ${SRC_FILE})

TARGET_LINK_LIBRARIES(${OBJECT} protobuf-c grpc grpc-c)

# add the install targets
install (TARGETS ${OBJECT} DESTINATION bin)
