#ifndef _ECS
#define _ECS

#include "visual.h"
#include "rect.h"
#include "define.h"

#include "sparse.h"

#include <stddef.h>

enum {
    SLEEP,
    SHOT,
    SHIELD,
    SPEED
};

typedef struct {
    int *entities;

    sparse_set* vc; // VISUAL COMPONENT
    sparse_set* pc; // PHYSICS COMPONENT
    sparse_set* hc; // HEALTH COMPONENT
    sparse_set* prjc; // PROJECTILE COMPONENT

    // POWER UPS
    sparse_set* type;
    sparse_set* sleep_c; // is sleeping
    sparse_set* shooting_c; // shooting level
    sparse_set* shield_c; // shield level
} ECS;

ECS* create_ecs(size_t max_entities);
void destroy_entity(ECS* ecs, int id);

#endif