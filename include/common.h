#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <stdlib.h>

#define UNUSED(x) ((void) x)

static inline char * dup(const char * xs) {
    size_t size = strlen(xs) + 1;

    char * ys = malloc(size);
    return ys == NULL ? NULL : memcpy(ys, xs, size);
}

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

#endif
