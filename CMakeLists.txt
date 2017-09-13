cmake_minimum_required(VERSION 2.8)
project(lwhttp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

if (${DEBUG})
    set(CMAKE_C_FLAGS "-D_DEBUG -g")
endif()

set(LWHTTP_SOURCES
        src/http_client.c
        src/http_client.h
        src/http_message.c
        src/http_message.h
        src/http_parser.c
        src/http_parser.h
        src/tcp_client.h
        src/tcp_client.c
        src/ss.c
        src/ss.h
        )

add_library(liblwhttp STATIC ${LWHTTP_SOURCES})

set (HTTP_CLIENT_TEST
        src/http_client_test.c
        )

add_executable(http_client_test ${HTTP_CLIENT_TEST})
target_link_libraries(http_client_test liblwhttp)

set (TCP_CLIENT_TEST
        src/tcp_client_test.c
        )

add_executable(tcp_client_test ${TCP_CLIENT_TEST})
target_link_libraries(tcp_client_test liblwhttp)
