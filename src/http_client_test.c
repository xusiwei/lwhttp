#include "http_client.h"
#include <string.h>

char buffer[4096];

int main(int argc, char* argv[])
{
    const char* url = "http://example.com/";
    enum http_method method = HTTP_GET;
    http_client http_cli, *client;
    http_message response;

    if (argc > 1) {
        url = argv[1];
    }
    if (argc > 2) {
        for (int i = 0; i <= HTTP_UNLINK; i++) {
            if (strcmp(http_method_str(i), argv[2]) == 0) {
                method = (enum http_method)i;
                break;
            }
        }
    }
    printf("method: %s\n", http_method_str(method));
    printf("URL: %s\n", url);

    client = &http_cli;

    http_client_init(client);
    http_client_set_request_buffer(client, buffer, sizeof(buffer));
    http_client_set_response_buffer(client, buffer, sizeof(buffer));

    http_client_add_header(client, "User-Agent", "curl/7.47.0");
    http_client_add_header(client, "Accept", "*/*");
    http_client_set_url(client, url);
    http_client_set_method(client, method);

    http_client_execute(client, &response);

    return 0;
}
