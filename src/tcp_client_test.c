#include <stdio.h>

#include "tcp_client.h"

#define HOST "emdroid.org"
#define PORT 80

const char REQUEST[] = "GET / HTTP/1.1\r\n"
                       "Host: " HOST "\r\n"
                       "User-Agent: curl/7.47.0\r\n"
                       "Accept: */*\r\n"
                       "\r\n";

char buffer[1024];

int main()
{
    long count = 0;
    tcp_client client, *cli;

    cli = &client;

    tcp_client_init(cli);

    tcp_client_connect(cli, HOST, PORT);

    printf("sent: %ld\n", tcp_client_send(cli, REQUEST, sizeof(REQUEST)));

    while ((count = tcp_client_recv(cli, buffer, sizeof(buffer))) > 0) {
        buffer[count] = '\0';
        printf("===(%ld)> %s<====\n", count, buffer);
    }

    tcp_client_close(cli);
    return 0;
}