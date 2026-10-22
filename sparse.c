#include "sparse.h"

#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

sparse_set *create_sparse_set(long int component_size, long int max_size) {
    if (component_size <= 0) {
        printf("(sparse.c: create_sparse_set) component_size must be bigger than 0\n");
        return NULL;
    }

    if (max_size <= 0) {
        printf("(sparse.c: create_sparse_set) max_size must be bigger than 0\n");
        return NULL;
    }

    sparse_set* set = NULL;

    set = (sparse_set*)xmalloc(sizeof(sparse_set));
    set->sparse = (int*)xmalloc(sizeof(int) * max_size);
    set->dense = (int*)xmalloc(sizeof(int) * max_size);
    set->component = (void*)xmalloc(component_size * max_size);

    set->component_size = component_size;
    set->size = 0;

    for (int i = 0; i < max_size; ++i) {
        set->sparse[i] = -1;
        set->dense[i] = -1;
    }

    return set;
}

void destroy_sparse_set(sparse_set* set) {
    if (!set)
        return;

    if (set->sparse) {
        free(set->sparse);
        set->sparse = NULL;
    }

    if (set->dense) {
        free(set->dense);
        set->dense = NULL;
    }

    if (set->component) {
        free(set->component);
        set->component = NULL;
    }

    free(set);
}

void add_component_to_id(sparse_set* set, void* component_info, int id) {
    if (set->sparse[id] >= 0 || id < 0)
        return;

    set->sparse[id] = set->size;
    set->dense[set->size] = id;
    memcpy((char*)(set->component + set->size * set->component_size), (char*)component_info, set->component_size);
    set->size += 1;
}

void remove_component_from_id(sparse_set *set, int id) {
    if (set->sparse[id] < 0 || id < 0)
        return;

    int pos = set->sparse[id];
    set->dense[pos] = set->dense[set->size - 1];
    set->dense[set->size - 1] = -1;
    set->sparse[set->dense[pos]] = pos;
    // memcpy((char*)&(((char*)set->component)[pos]), (char*)&(((char*)set->component)[set->size - 1]), set->component_size);
    memcpy(
        (char*)(set->component + pos * set->component_size),
        (char*)(set->component + (set->size - 1) * set->component_size),
        set->component_size
    );
    set->size -= 1;
}

void *get_component_from_id(sparse_set* set, int id) {
    if (set->sparse[id] < 0 || id < 0)
        return NULL;

    // return &((char*)set->component)[set->sparse[id]];
    return (char*)(set->component + set->sparse[id] * set->component_size);
}