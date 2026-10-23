#include "entities.h"

#include "visual.h"
#include "rect.h"
#include "health.h"

#include <stdlib.h>

static int find_free_id(ECS* ecs) {
    int id = -1;
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (ecs->entities[i] == 0) {
            id = i;
            break;
        }
    }
    return id;
}

void create_player(ECS* ecs) {
    ecs->entities[PLAYER] = 1;

    visual_component vc = {
        1, 0xFF0000FF
    };

    add_component_to_id(ecs->vc, &vc, PLAYER);

    rect_component rc = {
        150, 50,
        16, 16,
        0, 0,
        5
    };

    add_component_to_id(ecs->pc, &rc, PLAYER);

    health_component hc = {
        3, 0, 30
    };

    add_component_to_id(ecs->hc, &hc, PLAYER);

    int shooting_level = 1;
    add_component_to_id(ecs->shooting_c, &shooting_level, PLAYER);
}

void create_alien(ECS* ecs) {
    int id = find_free_id(ecs);
    if (id < 0)
        return;

    ecs->entities[id] = 1;

    visual_component vc = {
        1, 0xFF00FF00
    };

    add_component_to_id(ecs->vc, &vc, id);

    float vx = rand() % 2;
    float vy = rand() % 2;

    if (!vx) vx = -1;
    if (!vy) vy = -1;

    // rect_component rc = {
    //     rand() % 464, rand() % 258,
    //     16, 16,
    //     vx, vy,
    //     5
    // };

    rect_component rc = {
        0, 0,
        16, 16,
        vx, vy,
        5
    };

    add_component_to_id(ecs->pc, &rc, id);

    health_component hc = {
        3, 0, 30
    };

    add_component_to_id(ecs->hc, &hc, id);
}

void create_powerup(ECS* ecs, int type) {
    int id = find_free_id(ecs);
    if (id < 0)
        return;

    ecs->entities[id] = 1;

    visual_component vc = {
        1, 0xFFFF0000
    };

    add_component_to_id(ecs->vc, &vc, id);

    // rect_component rc = {
    //     rand() % 464, rand() % 258,
    //     16, 16,
    //     0, 0,
    //     5
    // };

    rect_component rc = {
        60, 60,
        16, 16,
        0, 0,
        5
    };

    add_component_to_id(ecs->pc, &rc, id);

    health_component hc = {
        3, 0, 30
    };

    add_component_to_id(ecs->hc, &hc, id);

    add_component_to_id(ecs->type, &type, id);
}