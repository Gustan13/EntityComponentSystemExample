#include "gu.h"

#include "define.h"

#include <pspuser.h>
#include <pspgu.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>

char list[0x20000] __attribute__((aligned(64)));
void *fbp0;
void *fbp1;


int exit_callback(int arg1, int arg2, void *common)
{

    sceKernelExitGame();
    return 0;

}

int callback_thread(unsigned int args, void *argp)
{

    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, (void*)0);

    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();

    return 0;

}

int setup_callbacks(void)
{

    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);

    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);

    return thid;

}

void initGu()
{

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

void endGu()
{

    sceGuDisplay(GU_FALSE);
    sceGuTerm();

}

void startFrame()
{

    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(0xFFFFFFFF); // White background
    sceGuClear(GU_COLOR_BUFFER_BIT);

}

void endFrame()
{

    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();

}

