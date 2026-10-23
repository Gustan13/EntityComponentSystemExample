#include "ecs.h"
#include "entities.h"
#include "define.h"
// #include "ppsspp_emu_api.h"

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

#include <pspuser.h>
#include <pspgu.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
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

// int print_hello() {
//     if (ppsspp_is_emulator()) {
//         ppsspp_send_output_str("Hello!\n");
//         return 1;
//     }
//     return 0;
// }

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

// Texture * loadTexture(const char * filename) {
//     Texture * texture = (Texture *) calloc(1, sizeof(Texture));

//     texture->data = (uint32_t *) stbi_load(filename, &(texture->width), &(texture->height), NULL, STBI_rgb_alpha);

//     // Make sure the texture cache is reloaded
//     sceKernelDcacheWritebackInvalidateAll();

//     return texture;
// }

void drawText(Texture* texture, float x, float y) {

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

    // sceGuTexMode(GU_PSM_8888, 0, 0, GU_FALSE);
    // sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
    // sceGuTexImage(0, texture->width, texture->height, texture->width, texture->data);

    // sceGuEnable(GU_TEXTURE_2D); 
    // sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuColor(0xFFFFFFFF);
    
    // sceGuDisable(GU_TEXTURE_2D);
}

typedef struct {
    unsigned short u, v;
    short x, y, z;
} Vertex_t;

void drawRect(visual_component* vc, rect_component* rc) {
    if (!vc->visible)
        return;

    static Vertex_t vertices[2];

    vertices[0].x = rc->x;
    vertices[0].y = rc->y;

    vertices[1].x = rc->x + rc->w;
    vertices[1].y = rc->y + rc->h;

    sceGuColor(vc->color);
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
    // sceGuDrawBuffer()
}

int main(void)  {
    SceCtrlData pad;

    // Use above functions to make exiting possible
    setup_callbacks();

    pspDebugScreenInit();

    ECS* ecs = create_ecs(MAX_ENTITIES);

    create_player(ecs);
    create_alien(ecs);
    create_powerup(ecs, SLEEP);


    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    initGu();
    running = 1;

    while(1) {

        pspDebugScreenSetXY(0, 2);

        sceCtrlReadBufferPositive(&pad, 1);

        pspDebugScreenPrintf("Analog X = %f, ", (float)(pad.Lx) - 128);
        pspDebugScreenPrintf("Analog Y = %f, \n", (float)(pad.Ly) - 128);

        if (ecs->entities[PLAYER])
        {

            rect_component* rc = get_component_from_id(ecs->pc, PLAYER);

            rc->vx = rc->vy = 0;

            float lx = (float)(pad.Lx) - 128;
            float ly = (float)(pad.Ly) - 128;

            if (lx)
            {
                rc->vx = lx / 128;
            }

            if (ly)
            {
                rc->vy = ly / 128;
            }

        }

        startFrame();

        for (int i = 0; i < MAX_ENTITIES; ++i)
        {

            if (!ecs->entities[i])
                continue;
            
            visual_component *vc = (visual_component*)get_component_from_id(ecs->vc, i);
            rect_component* rc = (rect_component*)get_component_from_id(ecs->pc, i);
         
            // MOVEMENT SYSTEM
            if (rc)
            {

                rc->x += rc->vx;
                rc->y += rc->vy;
                
                if (i != PLAYER)
                {

                    if (rc->x + rc->w > SCREEN_WIDTH || rc->x < 0)
                    {
                        rc->vx *= -1;
                        rc->x += rc->vx;
                    }

                    if (rc->y + rc->h > BUFFER_HEIGHT || rc->y < 0)
                    {
                        rc->vy *= -1;
                        rc->y += rc->vy;
                    }

                }

            }

            // RENDERING SYSTEM
            if (vc && rc)
                drawRect(vc, rc);

        }

        endFrame();
    }

    return 0;
}
