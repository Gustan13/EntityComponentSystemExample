#include "memory.h"

#include <stdio.h>

void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Error: Memory allocation failed (out of memory).\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}