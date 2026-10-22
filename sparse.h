#ifndef _SPARSE
#define _SPARSE

typedef struct {
    long int size;
    int *sparse;
    int *dense;

    long int component_size;
    void *component;
} sparse_set;

sparse_set *create_sparse_set(long int component_size, long int max_size);
void destroy_sparse_set(sparse_set* set);

void add_component_to_id(sparse_set* set, void* component_info, int id);
void remove_component_from_id(sparse_set *set, int id);

void *get_component_from_id(sparse_set* set, int id);

#endif