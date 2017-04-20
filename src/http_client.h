#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "http_message.h"
#include "http_parser.h"
#include "tcp_client.h"
#include <stdbool.h>

typedef struct http_client http_client;

#ifndef HTTP_MAX_QUERIES
#define HTTP_MAX_QUERIES 64
#endif // HTTP_MAX_QUERIES

#ifndef HTTP_NAME_MAX
#define HTTP_NAME_MAX 16
#endif // HTTP_NAME_MAX

#ifndef HTTP_VALUE_MAX
#define HTTP_VALUE_MAX 32
#endif // HTTP_VALUE_MAX

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 64
#endif // HOST_NAME_MAX

struct http_client {
    uint16_t http_major;
    uint16_t http_minor;

    tcp_client connector;

    char host[HOST_NAME_MAX];
    uint16_t port;

    struct http_message request;
    struct http_message* response;

    size_t request_buffer_used;
    struct ss request_buffer;
    struct ss response_buffer;

    struct uri_query queries;

    uint16_t nqueries;
    struct ss query_names[HTTP_MAX_QUERIES];
    struct ss query_values[HTTP_MAX_QUERIES];

    // ?name1=value1&name2=value2 ...
    char query_buffer[URI_MAX_QUERIES * (HTTP_NAME_MAX + 2 + HTTP_VALUE_MAX)];
    size_t query_buffer_used;

    // for parser
    http_parser parser;
    http_parser_settings settings;

    bool response_complete;
    struct ss last_header_field;
};

void http_client_init(http_client* client);
void http_client_set_request_buffer(http_client* client, const char* p,
                                    size_t len);
void http_client_set_response_buffer(http_client* client, const char* p,
                                     size_t len);
void http_client_set_http_version(http_client* client, unsigned short major,
                                  unsigned short minor);

void http_client_set_url(http_client* client, const char* url);
void http_client_set_method(http_client* client, uint16_t method);
void http_client_add_header(http_client* client, const char* name,
                            const char* value);
void http_client_add_query(http_client* client, const char* name,
                           const char* value);

int http_client_execute(http_client* client, http_message* response);

#endif // _HTTP_CLIENT_H_
