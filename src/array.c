#include <array.h>

Array newArray(size_t size) {
    if (size == 0) return (Array) {.data = NULL, .size = 0};

    void * data = malloc(size * sizeof(void *));
    return (Array) {.data = data, .size = size};
}

void freeArray(Array * A) {
    if (A->data == NULL) return;

    free(A->data);
    A->size = 0;
}
