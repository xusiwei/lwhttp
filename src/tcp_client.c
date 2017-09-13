#include "tcp_client.h"

#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

void tcp_client_init(tcp_client* client) { memset(client, 0, sizeof(*client)); }

static uint32_t resolve_host_addr(const char* host)
{
    uint32_t dest = 0;
#ifdef CONFIG_CC3200SDK_SIMPLELINK
    sl_NetAppDnsGetHostByName((_i8*)host, strlen(host), (_u32*)&dest,
                              SL_AF_INET);
    dest = htonl(dest);
#else  // CONFIG_CC3200SDK_SIMPLELINK
    struct hostent* he = gethostbyname(host);
    if (he && he->h_addr_list && he->h_addr_list[0]) {
        dest = ((struct in_addr*)(he->h_addr_list[0]))->s_addr;
    }
#endif // CONFIG_CC3200SDK_SIMPLELINK
    //  printf("dest: %d.%d.%d.%d\n", dest & 0xFF, (dest & 0xFF00)>>8, (dest
    //  & 0xFF0000)>>16, (dest & 0xFF000000)>>24);
    return dest;
}

int tcp_client_connect(tcp_client* client, const char* name, uint16_t port)
{
    int ret;
    struct sockaddr_in server_addr;

    client->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->fd < 0) {
        perror("socket");
    }

    client->port = port;
    client->host = resolve_host_addr(name);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(client->port);
    server_addr.sin_addr.s_addr = client->host;

    ret = connect(client->fd, (const struct sockaddr*)&server_addr,
                  sizeof(server_addr));
    if (ret < 0) {
        perror("connect");
    }
    return ret;
}

long tcp_client_send(tcp_client* client, const void* buf, size_t len)
{
    return send(client->fd, buf, len, 0);
}

long tcp_client_recv(tcp_client* client, void* buf, size_t len)
{
    return recv(client->fd, buf, len, 0);
}

int tcp_client_close(tcp_client* client) { return close(client->fd); }
