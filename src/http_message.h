#ifndef LWHTTP_HTTP_MESSAGE_H
#define LWHTTP_HTTP_MESSAGE_H

#include <stdint.h>
#include <stdio.h>

#include "ss.h"

#ifndef HTTP_MAX_HEADERS
#define HTTP_MAX_HEADERS 32
#endif

typedef struct uri_query uri_query;

typedef struct http_message http_message;

struct http_message {
    struct ss url;
    uint16_t method; /* requests only */
    uint16_t status; /* responses only */

    uint64_t content_length;
    uint16_t nheaders;
    struct ss header_names[HTTP_MAX_HEADERS];
    struct ss header_values[HTTP_MAX_HEADERS];

    struct ss body;
    struct ss chunk;

    uri_query* query; /* optional */
};

void http_message_init(http_message* msg);

void http_message_set_url(http_message* msg, const char* url);

void http_message_set_method(http_message* msg, uint16_t method);

void http_message_add_header(http_message* msg, const char* name,
                             const char* value);

void http_message_add_parameter(http_message* msg, const char* name,
                                const char* value);

void http_message_bind_query(http_message* msg, uri_query* query);

void http_message_add_ss_header(http_message* msg, struct ss name,
                                struct ss value);

#ifndef URI_QUERY_MAX
#define URI_QUERY_MAX 64
#endif // URI_QUERY_MAX

#ifndef URI_QUERY_NAME_MAX
#define URI_QUERY_NAME_MAX 16
#endif // URI_QUERY_NAME_MAX

#ifndef URI_QUERY_VALUE_MAX
#define URI_QUERY_VALUE_MAX 32
#endif // URI_QUERY_VALUE_MAX

struct uri_query {
    uint16_t nqueries;
    struct ss query_names[URI_QUERY_MAX];
    struct ss query_values[URI_QUERY_MAX];
};

size_t uri_query_build(uri_query* query, char* buf, size_t len);

#endif // LWHTTP_HTTP_MESSAGE_H
