#include "ss.h"

#include <string.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

char* ss_strcopy(char* buf, size_t len, struct ss src)
{
    size_t i;
    len = MIN(len, src.len);
    for (i = 0; i < len; i++) {
        buf[i] = src.p[i];
    }
    buf[i] = '\0';
    return buf;
}

int ss_strncmp(const struct ss* s1, const char* s2, size_t len)
{
    return strncmp(s1->p, s2, len);
}

int ss_strncasecmp(const struct ss* s1, const char* s2, size_t len)
{
    return strncasecmp(s1->p, s2, len);
}
