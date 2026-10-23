#ifndef _GU
#define _GU

void initGu();
void endGu();

void startFrame();
void endFrame();

int exit_callback(int arg1, int arg2, void *common);

int callback_thread(unsigned int args, void *argp);

int setup_callbacks(void);

#endif