
#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>

#include "intraLog.h"
#include "test.h"

PSP_MODULE_INFO("Hello Log", 0,1,0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

static unsigned int __attribute__((aligned(16))) list[262144];

FPS_COUNTER g_fps;

int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();
	return 0;
}


int callback_thread(SceSize args, void *argp)
{
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

int main(void)
{

	setup_callbacks();
	sceGuInit();
	Log_Init(); // Init Logging 
	sceGuStart(GU_DIRECT, list);
	
	sceGuDrawBuffer(GU_PSM_8888, (void*)0, 512);
	sceGuDispBuffer(480, 272, (void*)0x88000,512);
	sceGuDepthBuffer((void*)0x110000,512);
	

	sceGuOffset(2048 - (480/2), 2048 - (272/2));
	sceGuViewport(2048, 2048, 480, 272);
	sceGuScissor(0,0,480,272);
	sceGuEnable(GU_SCISSOR_TEST);

	sceGuDepthRange(65535,0);
	sceGuDisable(GU_DEPTH_TEST);

	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	while(1)
	{
		sceGuStart(GU_DIRECT, list);

		sceGuClearColor(0xFF003366);
		sceGuClear(GU_COLOR_BUFFER_BIT);
		
		if (g_fps.tick())
		{Log_UpdateFPS(g_fps.fps);}


		testLog();

		Log_DrawFPSOverlay(8.0f, 20.0f, 1.0f);
		Log_DrawOverlay(8.0f, 40.0f, 1.0f);

		

		sceGuFinish();
		sceGuSync(0,0);
		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
	}
	Log_Shutdown();
	sceGuTerm();
	return 0;
}