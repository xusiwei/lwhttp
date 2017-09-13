#include "http_message.h"

#include <string.h>

void http_message_init(http_message* msg) { memset(msg, 0, sizeof(*msg)); }

void http_message_bind_query(http_message* msg, uri_query* query)
{
    msg->query = query;
}

void http_message_add_parameter(http_message* msg, const char* name,
                                const char* value)
{
    uri_query* query = msg->query;
    uint16_t nqueries = query->nqueries;
    query->query_names[nqueries].p = name;
    query->query_names[nqueries].len = strlen(name);
    query->query_values[nqueries].p = value;
    query->query_values[nqueries].len = strlen(value);
    query->nqueries++;
}

void http_message_add_ss_header(http_message* msg, struct ss name,
                                struct ss value)
{
    uint16_t nheaders = msg->nheaders;
    msg->header_names[nheaders] = name;
    msg->header_values[nheaders] = value;
    msg->nheaders++;
}

void http_message_add_header(http_message* msg, const char* name,
                             const char* value)
{
    struct ss n = {name, strlen(name)};
    struct ss v = {value, strlen(value)};
    http_message_add_ss_header(msg, n, v);
}

void http_message_set_url(http_message* msg, const char* url)
{
    struct ss us = {url, strlen(url)};
    msg->url = us;
}

void http_message_set_method(http_message* msg, uint16_t method)
{
    msg->method = method;
}

#define _BUFFER_APPEND(buffer, used, p, len)                                   \
    do {                                                                       \
        memcpy(((char*)buffer) + used, p, len);                                \
        used += len;                                                           \
    } while (0)

size_t uri_query_build(uri_query* query, char* buf, size_t len)
{
    int i;
    struct ss* names = query->query_names;
    struct ss* values = query->query_values;
    size_t used = 0;
    for (i = 0; i < query->nqueries; i++) {
        _BUFFER_APPEND(buf, used, (i ? "&" : ""), 1);
        _BUFFER_APPEND(buf, used, names[i].p, names[i].len);
        _BUFFER_APPEND(buf, used, "=", 1);
        _BUFFER_APPEND(buf, used, values[i].p, values[i].len);
    }
    return used;
}