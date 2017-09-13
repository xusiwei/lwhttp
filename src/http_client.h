#ifndef LWHTTP_HTTP_CLIENT_H
#define LWHTTP_HTTP_CLIENT_H

#include <stdbool.h>

#include "http_message.h"
#include "http_parser.h"
#include "tcp_client.h"

typedef struct http_client http_client;

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif // HOST_NAME_MAX

struct http_client {
    uint16_t http_major;
    uint16_t http_minor;

    tcp_client connector;

    char host[HOST_NAME_MAX];
    uint16_t port;

    struct http_message* request;
    struct http_message* response;

    size_t request_buffer_used;
    struct ss request_buffer;
    struct ss response_buffer;

    // for parser
    http_parser parser;
    http_parser_settings settings;

    bool response_complete;
    struct ss last_header_field;

    struct uri_query query_stub;
    size_t query_buffer_used;
    // name1=value1&name2=value2 ...
    char query_buffer[URI_QUERY_MAX *
                      (URI_QUERY_NAME_MAX + 2 + URI_QUERY_VALUE_MAX)];
};

void http_client_init(http_client* client, http_message* request);
void http_client_set_request_buffer(http_client* client, const char* p,
                                    size_t len);
void http_client_set_response_buffer(http_client* client, const char* p,
                                     size_t len);
void http_client_bind_request(http_client* client, http_message* request);

void http_client_set_http_version(http_client* client, unsigned short major,
                                  unsigned short minor);

int http_client_execute(http_client* client, http_message* response);

#endif // LWHTTP_HTTP_CLIENT_H
