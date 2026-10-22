#include "ecs.h"

#include "memory.h"
#include "visual.h"
#include "rect.h"
#include "health.h"

#include <stdio.h>

ECS* create_ecs(size_t max_entities) {
    ECS* ecs = (ECS*)xmalloc(sizeof(ECS));

    ecs->entities = (int*)malloc(sizeof(int) * max_entities); 

    ecs->vc = create_sparse_set(sizeof(visual_component), max_entities);

    ecs->pc = create_sparse_set(sizeof(rect_component), max_entities);
    ecs->hc = create_sparse_set(sizeof(health_component), max_entities);
    ecs->prjc = create_sparse_set(sizeof(int), max_entities);

    ecs->type = create_sparse_set(sizeof(int), max_entities);
    ecs->sleep_c = create_sparse_set(sizeof(int), max_entities);
    ecs->shield_c = create_sparse_set(sizeof(int), max_entities);
    ecs->shooting_c = create_sparse_set(sizeof(int), max_entities);

    return ecs;
}

void destroy_entity(ECS* ecs, int id) {
    if (!ecs)
        return;

    ecs->entities[id] = 0;
    remove_component_from_id(ecs->vc, id);
    remove_component_from_id(ecs->pc, id);
    remove_component_from_id(ecs->hc, id);
    remove_component_from_id(ecs->prjc, id);

    remove_component_from_id(ecs->shooting_c, id);
    remove_component_from_id(ecs->shield_c, id);
    remove_component_from_id(ecs->sleep_c, id);
}