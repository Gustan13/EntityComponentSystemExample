#include "draw.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <pspgu.h>
#include <pspdisplay.h>
#include <pspuser.h>

typedef struct {
    unsigned short u, v;
    short x, y, z;
} Vertex_t;

typedef struct {
    float u, v;
    uint32_t color;
    float x, y, z;
} Vertex;

void drawRect(visual_component* vc, rect_component* rc)
{

    if (!vc->visible)
        return;

    static Vertex_t vertices[2];

    vertices[0].x = rc->x;
    vertices[0].y = rc->y;

    vertices[1].x = rc->x + rc->w;
    vertices[1].y = rc->y + rc->h;

    sceGuColor(vc->color);
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);

}

void drawTextured(Texture* texture, visual_component* vc, rect_component* rc)
{

    static Vertex vertices[2];

    float x = rc->x;
    float y = rc->y;
    
    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;
    vertices[0].color = vc->color;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;

    vertices[1].x = x + rc->w;
    vertices[1].y = y + rc->h;
    vertices[1].z = 0.0f;
    vertices[1].color = vc->color;
    vertices[1].u = texture->width;
    vertices[1].v = texture->height;

    sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
    sceGuTexImage(0, texture->width, texture->height, texture->width, texture->data);

    sceGuEnable(GU_TEXTURE_2D); 
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuDisable(GU_TEXTURE_2D);

}

static void drawDigit(Texture* texture, int digit, int x, int y)
{

    static Vertex v[2];

    digit = digit % 10;

    int u = (digit % 4) * 16;
    int vs = (digit / 4) * 16;

    v[0].x = x;
    v[0].y = y;
    v[0].z = 0.0f;
    v[0].color = 0xFFFFFFFF;
    v[0].u = u;
    v[0].v = vs;

    v[1].x = x + 16;
    v[1].y = y + 16;
    v[1].z = 0.0f;
    v[1].color = 0xFFFFFFFF;

    v[1].u = u + 16;
    v[1].v = vs + 16;

    sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexImage(0, texture->width, texture->height, texture->width, texture->data);

    sceGuEnable(GU_TEXTURE_2D);
    sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, v);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDisable(GU_BLEND);

}

void drawNumber(Texture* texture, int number, int x, int y)
{

    while (number != 0)
    {

        int digit = number % 10;
        number = number / 10;

        x -= 16;

        drawDigit(texture, digit, x, y);

    }

}

Texture * loadTexture(const char * filename)
{

    Texture * texture = (Texture *) calloc(1, sizeof(Texture));

    texture->data = (int *) stbi_load(filename, &(texture->width), &(texture->height), NULL, STBI_rgb_alpha);

    sceKernelDcacheWritebackInvalidateAll();

    return texture;

}