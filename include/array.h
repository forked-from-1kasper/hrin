#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

typedef struct {
    void ** data;
    size_t size;
} Array;

Array newArray(size_t);
void freeArray(Array *);

static inline Array * emptyArray()
{ return calloc(1, sizeof(Array)); }

static inline void extendArray(Array * A, size_t size) {
    if (A->size < size) {
        A->data = realloc(A->data, size * sizeof(void *));
        A->size = size;
    }
}

static inline Array sliceArray(Array * A, size_t i, size_t j)
{ return (Array) {.data = A->data == NULL ? NULL : A->data + i, .size = j - i}; }

static inline void * getArray(Array * A, size_t index)
{ return A->data[index]; }

static inline void setArray(Array * A, size_t index, void * value)
{ A->data[index] = value; }

#endif
