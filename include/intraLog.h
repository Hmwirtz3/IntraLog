#pragma once

#include <cstdarg>
#include <pspkernel.h>

void Log_Init();
void Log_Shutdown();
void Log(const char* format, ...);
void Log_DrawOverlay(float x, float y, int maxLines = 20, float scale = 0.7f);
void Log_UpdateFPS(float fps);
void Log_Clear();
void Log_SetEnabled(bool enabled);
bool Log_IsEnabled();
void Log_DrawFPSOverlay(float x, float y, float scale);

struct FPS_COUNTER
{
	uint64_t lastTick = 0;
	uint32_t frames   = 0;
	float    fps      = 0.0f;

	bool tick()
	{
		uint64_t now = sceKernelGetSystemTimeWide();
		if (!lastTick) lastTick = now;
		frames++;
		uint64_t elapsed = now - lastTick;
		if (elapsed >= 1000000ULL)
		{
			fps = (float)frames * (1000000.0f / (float)elapsed);
			frames = 0;
			lastTick = now;
			return true;

		}
		return false;
	}
};

