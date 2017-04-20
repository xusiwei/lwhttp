#include "http_client.h"
#include "http_parser.h"

#include <ctype.h>
#include <string.h>

#define CRLF "\r\n"

#define DEBUG(fmt, ...) printf("%s: " fmt "\r\n", __func__, ##__VA_ARGS__)

void http_client_set_http_version(http_client* client, unsigned short major,
                                  unsigned short minor)
{
    client->http_major = major;
    client->http_minor = minor;
}

void http_client_set_request_buffer(http_client* client, const char* p,
                                    size_t len)
{
    client->request_buffer.p = p;
    client->request_buffer.len = len;
}

void http_client_set_response_buffer(http_client* client, const char* p,
                                     size_t len)
{
    client->response_buffer.p = p;
    client->response_buffer.len = len;
}

void http_client_set_url(http_client* client, const char* url)
{
    http_message_set_url(&client->request, url);
}
void http_client_set_method(http_client* client, uint16_t method)
{
    http_message_set_method(&client->request, method);
}

void http_client_add_header(http_client* client, const char* name,
                            const char* value)
{
    struct ss us_name = {name, strlen(name)};
    struct ss us_value = {value, strlen(value)};
    http_message_add_header(&client->request, us_name, us_value);
}

void http_client_add_query(http_client* client, const char* name,
                           const char* value)
{
    uint16_t nqueries = client->nqueries;
    client->query_names[nqueries].p = name;
    client->query_names[nqueries].len = strlen(name);
    client->query_values[nqueries].p = value;
    client->query_values[nqueries].len = strlen(value);
    client->nqueries++;
}

#define _BUFFER_APPEND(buffer, used, p, len)                                   \
    do {                                                                       \
        memcpy(((char*)buffer) + used, p, len);                                \
        used += len;                                                           \
    } while (0)

static void _request_buffer_append(http_client* client, const char* p,
                                   size_t len)
{
    char* buf = (char*)client->request_buffer.p;
    _BUFFER_APPEND(buf, client->request_buffer_used, p, len);
}

static void _build_query_string(http_client* client)
{
    int i;
    struct ss* names = client->query_names;
    struct ss* values = client->query_values;
    for (i = 0; i < client->nqueries; i++) {
        _BUFFER_APPEND(client->query_buffer, client->query_buffer_used,
                       (i ? "&" : ""), 1);
        _BUFFER_APPEND(client->query_buffer, client->query_buffer_used,
                       names[i].p, names[i].len);
        _BUFFER_APPEND(client->query_buffer, client->query_buffer_used, "=", 1);
        _BUFFER_APPEND(client->query_buffer, client->query_buffer_used,
                       values[i].p, values[i].len);
        //    _request_buffer_append(client, (i ? "&" : "?"), 1);
        //    _request_buffer_append(client, names[i].p, names[i].len);
        //    _request_buffer_append(client, "=", 1);
        //    _request_buffer_append(client, values[i].p,
        //    values[i].len);
    }
}

static void _prepare_resquest(http_client* client)
{
    int i;
    char proto[16];
    const char* metstr;
    enum http_method method;
    struct http_parser_url hp;
    struct ss *names, *values;
    struct ss us = client->request.url;
    static char number_text[16] = "";

    http_parser_url_init(&hp);
    http_parser_parse_url(us.p, us.len,
                          0, // TODO: confirm this argument
                          &hp);

    if (hp.port) {
        client->port = hp.port;
    } else {
        client->port = 80;
    }

    // prepare query string
    if (hp.field_set & UF_QUERY) {
        memcpy(client->query_buffer, us.p + hp.field_data[UF_QUERY].off,
               hp.field_data[UF_QUERY].len);
        client->query_buffer_used += hp.field_data[UF_QUERY].len;
    } else {
        _build_query_string(client);
    }

    // prepare HTTP Request Message
    // refs: http://www.tcpipguide.com/free/t_HTTPRequestMessageFormat.htm
    //
    // for request line
    // METHOD
    method = (enum http_method)client->request.method;
    metstr = http_method_str(method);
    _request_buffer_append(client, metstr, strlen(metstr));
    _request_buffer_append(client, " ", 1);

    // RESUOURCE
    if (hp.field_set & UF_PATH) {
        _request_buffer_append(client, us.p + hp.field_data[UF_PATH].off,
                               hp.field_data[UF_PATH].len);
    }
    if (HTTP_GET == method) {
        _request_buffer_append(client, "?", 1);
        _request_buffer_append(client, client->query_buffer,
                               client->query_buffer_used);
    } else {
        client->request.content_length = client->query_buffer_used;
        snprintf(number_text, sizeof(number_text), "%zd",
                 client->query_buffer_used);
        http_client_add_header(client, "Content-Length", number_text);
        http_client_add_header(client, "Content-Type",
                               "application/x-www-form-urlencoded");
    }
    _request_buffer_append(client, " ", 1);

    // SCHEMA
    for (i = 0; i < hp.field_data[UF_SCHEMA].len; i++) {
        proto[i] = (char)toupper(us.p[hp.field_data[UF_SCHEMA].off + i]);
    }
    proto[i++] = '/';
    proto[i++] = (char)('0' + (client->http_major % 10));
    proto[i++] = '.';
    proto[i++] = (char)('0' + (client->http_minor % 10));
    _request_buffer_append(client, proto, i);
    _request_buffer_append(client, CRLF, 2);

    // for request headers
    if (hp.field_set & UF_HOST) { // Host line
        _request_buffer_append(client, "Host: ", sizeof("Host: ") - 1);
        struct ss host = {us.p + hp.field_data[UF_HOST].off,
                          hp.field_data[UF_HOST].len};
        _request_buffer_append(client, host.p, host.len);
        memcpy(client->host, host.p, host.len);
        _request_buffer_append(client, CRLF, 2);
    }
    names = client->request.header_names;
    values = client->request.header_values;
    for (i = 0; i < client->request.nheaders; i++) {
        _request_buffer_append(client, names[i].p, names[i].len);
        _request_buffer_append(client, ": ", 2);
        _request_buffer_append(client, values[i].p, values[i].len);
        _request_buffer_append(client, CRLF, 2);
    }

    // for separation line
    _request_buffer_append(client, CRLF, 2);

    // for body
    if (HTTP_GET != (enum http_method)client->request.method) {
        _request_buffer_append(client, client->query_buffer,
                               client->query_buffer_used);
        _request_buffer_append(client, CRLF, 2);
        //    tcp_client_send(&client->connector, pos,
        //    client->query_buffer_used + 2);
    }
}

int http_client_execute(http_client* client, http_message* response)
{
    ssize_t nbytes = 0, count = 0, remains = 0;
    char* pos;

    client->response = response;
    _prepare_resquest(client);
    tcp_client_connect(&client->connector, client->host, client->port);

    count = 0;
    pos = (char*)client->request_buffer.p;
    remains = client->request_buffer_used;
    do {
        nbytes = tcp_client_send(&client->connector, pos, remains);
        remains -= nbytes;
        count += nbytes;
        pos += nbytes;
    } while (nbytes > 0 && remains > 0);

    DEBUG("sent count: %ld\n", count);

    // parser reset
    http_parser_init(&client->parser, HTTP_RESPONSE);
    client->parser.data = client;

    count = 0;
    pos = (char*)client->response_buffer.p;
    do {
        nbytes = tcp_client_recv(&client->connector, pos,
                                 client->response_buffer.len - count);
        if (nbytes <= 0) break;
        struct ss received = {pos, nbytes};
        http_parser_execute(&client->parser, &client->settings, pos, nbytes);
        count += nbytes;
        pos += nbytes;
    } while (!client->response_complete);
    DEBUG("recv count: %ld\n", count);

    tcp_client_close(&client->connector);
    return 0;
}

static int on_url(http_parser* parser, const char* at, size_t len)
{
    http_client* client = (http_client*)parser->data;
    struct ss s = {at, len};
    client->response->url = s;
    SS_DUMP(s);
    return 0;
}

static int on_status(http_parser* parser, const char* at, size_t len)
{
    http_client* client = (http_client*)parser->data;
    client->response->status = parser->status_code;
    struct ss s = {at, len};
    SS_DUMP(s);
    return 0;
}

static int on_body(http_parser* parser, const char* at, size_t len)
{
    http_client* client = (http_client*)parser->data;
    struct ss s = {at, len};
    client->response->body = s;
    SS_DUMP(s);
    return 0;
}

static int on_header_field(http_parser* parser, const char* at, size_t len)
{
    struct ss name = {at, len};
    http_client* client = (http_client*)parser->data;
    client->last_header_field = name;
    SS_DUMP(name);
    return 0;
}

static int on_header_value(http_parser* parser, const char* at, size_t len)
{
    struct ss value = {at, len};
    http_client* client = (http_client*)parser->data;
    http_message_add_header(client->response, client->last_header_field, value);
    SS_DUMP(value);
    return 0;
}

static int on_chunk_header(http_parser* parser)
{
    DEBUG("");
    return 0;
}

static int on_chunk_complete(http_parser* parser)
{
    DEBUG("");
    return 0;
}

static int on_headers_complete(http_parser* parser)
{
    http_client* client = (http_client*)parser->data;
    DEBUG("nheaders: %u", client->response->nheaders);
    if (client->parser.content_length) {
        DEBUG("content_length: %lu\n", client->parser.content_length);
        client->response->content_length = client->parser.content_length;
    }
    return 0;
}

static int on_message_begin(http_parser* parser)
{
    DEBUG("");
    return 0;
}

static int on_message_complete(http_parser* parser)
{
    http_client* client = (http_client*)parser->data;

    client->response_complete = true;
    DEBUG("true");
    return 0;
}

void http_client_init(http_client* client)
{
    http_parser_settings* settings = &client->settings;

    memset(client, 0, sizeof(http_client));
    http_parser_init(&client->parser, HTTP_RESPONSE);
    client->parser.data = client;

    http_parser_settings_init(settings);
    tcp_client_init(&client->connector);

    client->http_major = 1;
    client->http_minor = 1;

    settings->on_body = on_body;
    settings->on_chunk_header = on_chunk_header;
    settings->on_chunk_complete = on_chunk_complete;
    settings->on_header_field = on_header_field;
    settings->on_header_value = on_header_value;
    settings->on_headers_complete = on_headers_complete;
    settings->on_message_begin = on_message_begin;
    settings->on_message_complete = on_message_complete;
    settings->on_status = on_status;
    settings->on_url = on_url;
}
