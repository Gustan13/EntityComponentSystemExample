#ifndef _ENTITIES
#define _ENTITIES

#include "ecs.h"

void create_player(ECS* ecs);
void create_alien(ECS* ecs);
void create_powerup(ECS* ecs, int type);

#endif