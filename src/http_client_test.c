#include "http_client.h"
#include <string.h>

char buffer[4096];

int main(int argc, char* argv[])
{
    const char* url = "http://example.com/";
    enum http_method method = HTTP_POST;
    http_client http_cli, *client;
    http_message request, response;

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

    http_client_init(client, &request);
    http_client_set_request_buffer(client, buffer, sizeof(buffer));
    http_client_set_response_buffer(client, buffer, sizeof(buffer));

    http_message_add_header(&request, "User-Agent", "curl/7.47.0");
    http_message_add_header(&request, "Accept", "*/*");
    http_message_set_url(&request, url);
    http_message_set_method(&request, method);

    http_client_execute(client, &response);

    printf("==== response header ====\n");
    for (int i = 0; i < response.nheaders; i++) {
        static char key[64], val[64];
        ss_strcopy(key, sizeof(key), response.header_names[i]);
        ss_strcopy(val, sizeof(val), response.header_values[i]);
        printf("%s: %s\n", key, val);
    }

    static char body[4096];
    memset(body, 'X', sizeof(body));
    ss_strcopy(body, sizeof(body), response.body);
    printf("==== response body ====\n%s", body);
    printf("=======================\n");
    return 0;
}
