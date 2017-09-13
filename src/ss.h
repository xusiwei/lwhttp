//
// Created by xu on 17-4-20.
//

#ifndef LWHTTP_SS_H
#define LWHTTP_SS_H

#include <stddef.h>

// string slice
struct ss {
    const char* p;
    size_t len;
};

char* ss_strcopy(char* buf, size_t len, struct ss src);

int ss_strncmp(const struct ss* s1, const char* s2, size_t len);

int ss_strncasecmp(const struct ss* s1, const char* s2, size_t len);

#ifdef _DEBUG
#define SS_DUMP(s)                                                             \
    do {                                                                       \
        printf("%s| ", __func__);                                              \
        printf("%s(%zd): \'", #s, s.len);                                      \
        printf("%.*s", (int)s.len, s.p);                                       \
        printf("\'\n");                                                        \
    } while (0)
#else // _DEBUG
#define SS_DUMP(s)
#endif // _DEBUG

#endif // LWHTTP_SS_H
