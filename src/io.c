#include <stdlib.h>
#include <io.h>

static size_t capacity = 0, length = 0;
static char * outbuf = NULL;

void fpush(char value) {
    if (capacity <= length) {
        capacity += 512;
        outbuf = realloc(outbuf, capacity);
    }

    outbuf[length++] = value;
}

int fnext(FILE * stream) {
    int recv = fgetc(stream);
    fpush(recv);
    return recv;
}

char * fdup() {
    char * retbuf = realloc(outbuf, length);
    retbuf[length - 1] = '\0';

    capacity = length = 0; outbuf = NULL;
    return retbuf;
}

void fdrop() {
    capacity = length = 0;
    free(outbuf); outbuf = NULL;
}
