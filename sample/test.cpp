#include "intraLog.h"
#include <pspge.h>

void testLog()
{
    Log("This is a test Log from test.cpp");
    Log("This is a test of the wordwrapping that IntraLog Supports so easily");

    float speed = 42.7893f;
    Log("Speed: %.2f", speed);       
    Log("Speed (full precision): %f", speed);

    int lives = 3;
    Log("Lives: %d", lives);         
         

    uint32_t flags = 0xAF12FF00;
    Log("Flags: 0x%08X", flags);     


    void* vramPtr = sceGeEdramGetAddr();
    Log("VRAM pointer: %p", vramPtr);   

    float x = 100.0f, y = 200.5f;
    Log("Position = (%.1f, %.1f)", x, y);

    size_t size = 32768;
    Log("Chunk size: %zu bytes", size);
    Log("Chunk size: %.2f KB", size / 1024.0f);





}