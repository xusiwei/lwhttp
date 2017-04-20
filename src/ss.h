//
// Created by xu on 17-4-20.
//

#ifndef HTTP_CLIENT_USTR_H
#define HTTP_CLIENT_USTR_H

#include <stddef.h>

// string slice
struct ss {
    const char* p;
    size_t len;
};

#define SS_DUMP(s)                                                             \
    do {                                                                       \
        char fmt[16];                                                          \
        printf("%s| ", __func__);                                              \
        printf("%s: (%zd)\'", #s, s.len);                                      \
        snprintf(fmt, sizeof(fmt), "%%.%zds", s.len);                          \
        printf(fmt, s.p);                                                      \
        printf("\'\n");                                                        \
    } while (0)

#endif // HTTP_CLIENT_USTR_H
