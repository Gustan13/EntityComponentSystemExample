#include "ecs.h"
#include "entities.h"
#include "define.h"
#include "draw.h"
#include "gu.h"

#include <pspuser.h>
#include <pspgu.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <stdlib.h>

// PSP_MODULE_INFO is required
PSP_MODULE_INFO("Hello World", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int running;

int main(void)  {
    SceCtrlData pad;

    // Use above functions to make exiting possible
    setup_callbacks();

    pspDebugScreenInit();

    Texture* t = loadTexture("nums.png");

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

        int ent_count = 0;

        for (int i = 0; i < MAX_ENTITIES; ++i)
        {

            if (!ecs->entities[i])
                continue;

            ent_count++;
            
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

        drawNumber(t, 2009256, SCREEN_WIDTH, 0);

        endFrame();
    }

    return 0;
}
