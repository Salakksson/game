#include "msg.h"
#include <raylib.h>

static Music current;

void music_init()
{

    InitAudioDevice();

    current = LoadMusicStream(ASSETS_DIR"music/bg_p1.mp3");

	// PlayMusicStream(current);
}
void music_quit()
{
    UnloadMusicStream(current);
    CloseAudioDevice();
}
void music_main_loop()
{
#ifndef DEBUG
    // UpdateMusicStream(current);
#endif
}
