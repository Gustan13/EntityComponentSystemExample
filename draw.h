#ifndef _DRAW
#define _DRAW

#include "visual.h"
#include "rect.h"

typedef struct
{
    int width, height;
    int * data;
} Texture;


void drawRect(visual_component* vc, rect_component* rc);
void drawTextured(Texture* texture, visual_component* vc, rect_component* rc);
void drawNumber(Texture* texture, int number, int x, int y);
Texture * loadTexture(const char * filename);

#endif