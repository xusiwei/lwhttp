cmake_minimum_required(VERSION 2.8)
project(lwhttp)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(COMMON_SOURCES
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

set (HTTP_CLIENT_TEST
        ${COMMON_SOURCES}
        src/http_client_test.c
        )

add_executable(http_client_test ${HTTP_CLIENT_TEST})

set (TCP_CLIENT_TEST
        ${COMMON_SOURCES}
        src/tcp_client_test.c
        )

add_executable(tcp_client_test ${TCP_CLIENT_TEST})
