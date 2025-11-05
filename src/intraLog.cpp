#include "intraLog.h"

extern "C" {
#include "intraFont.h"
}

#include <pspgu.h>
#include <pspkernel.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <algorithm>




namespace 
{
	constexpr int LOG_MAX_LINES  = 128;
	constexpr int LOG_LINE_CHARS = 256;


	bool s_enabled = true;


	char s_lines[LOG_MAX_LINES][LOG_LINE_CHARS];
	int s_head = 0;
	int s_count = 0;


	intraFont* s_font = nullptr;
	bool       s_ready = false;


	inline void push_line(const char* text)
	{
		std::snprintf(s_lines[s_head], LOG_LINE_CHARS, "%s", text);
		s_head = (s_head + 1) % LOG_MAX_LINES;
		if (s_count < LOG_MAX_LINES) ++s_count;
	}
}




void Log_Clear()
{
	s_head = 0;
	s_count = 0;
}


void Log_SetEnabled(bool enabled)
{
	s_enabled = enabled;
}



bool Log_IsEnabled()
{
	return s_enabled;
}


void Log_Init()
{
	if (s_ready) return;

	intraFontInit();

	const char* candidates[] = 
	{"flash0:/font/ltn1.pgf",
	"flash0:font/ltn0.pgf",
	"flash0:font/ltn8.pgf"

	};

	for (const char* path : candidates)
	{
		s_font = intraFontLoad(path, INTRAFONT_CACHE_MED);
		if (s_font)
		{
			intraFontSetEncoding(s_font, INTRAFONT_STRING_UTF8);
			intraFontSetStyle(s_font, 0.7f, 0xffffffff, 0x00000000, 0.0f, 0);
			s_ready = true;
			break;
		}
	}

}


void Log_Shutdown()
{
	if (s_font) intraFontUnload(s_font);
	intraFontShutdown();
	s_font = nullptr;
	s_ready = false;
	s_head  = 0;
	s_count = 0;

}

void Log(const char* fmt, ...)
{
	if (!s_ready || !s_font || !s_enabled) return;
	char buf[LOG_LINE_CHARS];
	va_list ap; va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	push_line(buf);
}

namespace {

	constexpr float kScreenW = 480.0f;


	int DrawWrappedLine(intraFont* font,
						const char* text,
						float x,
						float y,
						float maxWidth,
						float scale,
						float baseDy)
	{
		int rows = 0;


		if(!text || !*text)
		{return 0;}

		char rowBuff[LOG_LINE_CHARS];

		const char* p = text;
		
		while (*p)
		{
			while (*p == ' ') ++p;
			if(!*p) break;


			size_t rowLen = 0;
			rowBuff[0] = '\0';

			const char* lineStart = p;
			const char* lastGoodBreak = nullptr;
			float lastGoodWidth = 0.0f;

			while (*p)
			{
				const char* wordStart = p;
				while(*p && *p != ' ') ++p;

				size_t addLen = (size_t)(p - lineStart);
				if (addLen >= sizeof(rowBuff)) addLen = sizeof(rowBuff) - 1;

				std::memcpy(rowBuff, lineStart, addLen);
                rowBuff[addLen] = '\0';

				float w = intraFontMeasureText(font, rowBuff);


				if (w <= maxWidth)
				{
					if (*p == ' ') {lastGoodBreak = p + 1; lastGoodWidth = w;}
					if (*p == ' ') ++p;

					continue;
				}
				else
				{
					if (lastGoodBreak)
					{
						size_t goodLen = (size_t)(lastGoodBreak - lineStart);
						if (goodLen >= sizeof(rowBuff)) goodLen = sizeof(rowBuff) - 1;
						std::memcpy(rowBuff, lineStart, goodLen);
						rowBuff[goodLen] = '\0';
						p = lastGoodBreak;
					}
					else
					{ 
						size_t lo = 1, hi = std::min(sizeof(rowBuff)-1, (size_t)std::strlen(lineStart));
						size_t cut = 1;
						while(lo <= hi)
						{
							size_t mid = (lo + hi) >> 1;
							std::memcpy(rowBuff, lineStart, mid);
							rowBuff[mid] = '\0';
							float wm = intraFontMeasureText(font, rowBuff);
							if (wm <= maxWidth) { cut = mid; lo = mid + 1; }
                            else { if (mid == 0) break; hi = mid - 1; }
                        }
                        std::memcpy(rowBuff, lineStart, cut);
                        rowBuff[cut] = '\0';
                        p = lineStart + cut;
					}

					intraFontPrint(font, x, y, rowBuff);
					y += baseDy;
                    ++rows;

					lineStart = p;
					lastGoodBreak = nullptr;
					lastGoodWidth = 0.0f;
				}
			}

			if (lineStart && *lineStart)
			{
				size_t remLen = std::min(sizeof(rowBuff)-1, std::strlen(lineStart));
				std::memcpy(rowBuff, lineStart, remLen);
				rowBuff[remLen] = '\0';
				intraFontPrint(font, x, y, rowBuff);
				y += baseDy;
				++rows;
			}
 

		}

		return rows;
	}
}


void Log_DrawOverlay(float x, float y, int maxLines, float scale)
{
	if (!s_ready || !s_font || s_count == 0) return;

	intraFontSetStyle(s_font, scale, 0xFFFFFFFF, 0x00000000, 0.0f, 0);

	int rowsBudget = (maxLines > 0) ? maxLines : s_count * 2;
	const float baseDy = 12.0f * (scale / 0.7f);
	const float maxWidth = std::max(0.0f, kScreenW - x - 4.0f);

	int start = (s_head - s_count + LOG_MAX_LINES) * LOG_MAX_LINES;

	for (int i =0; i < s_count && rowsBudget > 0; ++i)
	{
		int idx = (start + i ) % LOG_MAX_LINES;

		const char* whole = s_lines[idx];
		const char* seg = whole;

		while (rowsBudget > 0 && seg &&& *seg)
		{
			const char* nl = std::strchr(seg, '\n');
			if (nl)
			{
				char tmp[LOG_LINE_CHARS];
				size_t len = std::min<size_t>(nl - seg, sizeof(tmp) -1);
				std::memcpy(tmp, seg, len); tmp[len] = '\0';
				
				rowsBudget -= DrawWrappedLine(s_font, tmp, x, y, maxWidth, scale, baseDy);
				seg = nl +1;
			}
			else
			{
				rowsBudget -= DrawWrappedLine(s_font, seg, x, y, maxWidth, scale, baseDy);
				break;
			
			}
		}
	}
}

namespace 
{
	char s_fpsText[64] = "";
	bool s_fpsValid = false;
}

void Log_UpdateFPS(float fps)
{
	if (!s_ready || !s_font) return;
	std::snprintf(s_fpsText, sizeof(s_fpsText), "FPS: %.2f", fps);
	s_fpsValid = true;
}


void Log_DrawFPSOverlay(float x, float y, float scale) 
{
	if(!s_ready || !s_font || !s_fpsValid) return;

	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0,0);
	sceGuDisable(GU_DEPTH_TEST);
	sceGuDepthMask(GU_TRUE);

	intraFontSetStyle(s_font, scale, 0xFFFFFFFF, 0x00000000, 0.0f ,0);
	intraFontPrint(s_font, x, y, s_fpsText);
}