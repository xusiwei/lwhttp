#ifndef LWHTTP_TCP_CLIENT_H
#define LWHTTP_TCP_CLIENT_H

#include <stddef.h>
#include <stdint.h>

typedef struct tcp_client tcp_client;

struct tcp_client {
    int fd;
    uint32_t host;
    uint16_t port;
};

void tcp_client_init(tcp_client* client);

int tcp_client_connect(tcp_client* client, const char* host, uint16_t port);

long tcp_client_send(tcp_client* client, const void* buf, size_t len);

long tcp_client_recv(tcp_client* client, void* buf, size_t len);

int tcp_client_close(tcp_client* client);

#endif // LWHTTP_TCP_CLIENT_H
