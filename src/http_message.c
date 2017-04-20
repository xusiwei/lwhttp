#include "http_message.h"

#include <string.h>

void http_message_add_header(http_message* msg, struct ss name, struct ss value)
{
    uint16_t nheaders = msg->nheaders;
    msg->header_names[nheaders] = name;
    msg->header_values[nheaders] = value;
    msg->nheaders++;
}

void http_message_set_url(http_message* client, const char* url)
{
    struct ss us = {url, strlen(url)};
    client->url = us;
}

void http_message_set_method(http_message* client, uint16_t method)
{
    client->method = method;
}