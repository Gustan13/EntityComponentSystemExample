#include "ecs.h"
#include "entities.h"
#include "define.h"
#include "ppsspp_emu_api.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <pspuser.h>
#include <pspgu.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <stdlib.h>

// PSP_MODULE_INFO is required
PSP_MODULE_INFO("Hello World", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

// PSP_HEAP_SIZE_KB(4096);

#define BUFFER_WIDTH 512
#define BUFFER_HEIGHT 272
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT BUFFER_HEIGHT

char list[0x20000] __attribute__((aligned(64)));
int running;

void *fbp0;
void *fbp1;

typedef struct
{
    int width, height;
    uint32_t * data;
} Texture;

int print_hello() {
    if (ppsspp_is_emulator()) {
        ppsspp_send_output_str("Hello!\n");
        return 1;
    }
    return 0;
}

int exit_callback(int arg1, int arg2, void *common) {
    running = 0;
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

void initGu(){
    sceGuInit();

    fbp0 = guGetStaticVramBuffer(BUFFER_WIDTH, BUFFER_HEIGHT, GU_PSM_8888);
    fbp1 = guGetStaticVramBuffer(BUFFER_WIDTH, BUFFER_HEIGHT, GU_PSM_8888);

    //Set up buffers
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888,(void*)fbp0,BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,(void*)fbp1,BUFFER_WIDTH);
    sceGuDepthBuffer((void*)fbp0,BUFFER_WIDTH);

    //Set up viewport
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // //Set some stuff
    // sceGuDepthRange(65535, 0); //Use the full buffer for depth testing - buffer is reversed order

    // sceGuDepthFunc(GU_GEQUAL); //Depth buffer is reversed, so GEQUAL instead of LEQUAL
    // sceGuEnable(GU_DEPTH_TEST); //Enable depth testing

    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

void endGu(){
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

void startFrame(){
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0xFFFFFFFF); // White background
    sceGuClear(GU_COLOR_BUFFER_BIT);
}

void endFrame(){
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

typedef struct {
    float u, v;
    uint32_t color;
    float x, y, z;
} Vertex;

Texture * loadTexture(const char * filename) {
    Texture * texture = (Texture *) calloc(1, sizeof(Texture));

    texture->data = (uint32_t *) stbi_load(filename, &(texture->width), &(texture->height), NULL, STBI_rgb_alpha);

    // Make sure the texture cache is reloaded
    sceKernelDcacheWritebackInvalidateAll();

    return texture;
}

void drawRect(Texture* texture, float x, float y) {

    static Vertex vertices[2];

    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;
    vertices[0].color = 0xFFFFFFFF;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;

    vertices[1].x = x + texture->width;
    vertices[1].y = y + texture->height;
    vertices[1].z = 0.0f;
    vertices[1].color = 0xFFFFFFFF;
    vertices[1].u = texture->width;
    vertices[1].v = texture->height;

    sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
    sceGuTexImage(0, texture->width, texture->height, texture->width, texture->data);

    sceGuEnable(GU_TEXTURE_2D); 
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    
    sceGuDisable(GU_TEXTURE_2D);
}

int main(void)  {
    // Use above functions to make exiting possible
    setup_callbacks();

    ECS* ecs = create_ecs(MAX_ENTITIES);

    create_player(ecs);
    create_alien(ecs);
    create_powerup(ecs, SLEEP);

    Texture* texture = NULL;
    texture = loadTexture("grass.png");

    initGu();
    running = 1;

    while(1) {
        startFrame();

        for (int i = 0; i < MAX_ENTITIES; ++i) {
            if (!ecs->entities[i])
                continue;
            visual_component *vc = (visual_component*)get_component_from_id(ecs->vc, i);
            rect_component* rc = (rect_component*)get_component_from_id(ecs->pc, i);

            if (vc->visible)
            drawRect(texture, rc->x, rc->y);
        }

        endFrame();
    }

    return 0;
}
