#include "music.h"
#include <raylib.h>

static Music* current;

static Sound fx_intro;
static Sound fx_win;
static Music track_1;
static Music track_2;
static Music track_3;
void music_init()
{
    InitAudioDevice();

	track_1 = LoadMusicStream(MUSIC_DIR"tracks/music-1.wav");
	track_2 = LoadMusicStream(MUSIC_DIR"tracks/music-2.wav");
	track_3 = LoadMusicStream(MUSIC_DIR"tracks/music-3.wav");

	fx_intro = LoadSound(MUSIC_DIR"fx/intro.wav");
    fx_win = LoadSound(MUSIC_DIR"fx/win.wav");
	
	// PlaySound(fx_intro);
}
void music_quit()
{
    // UnloadMusicStream(current);
    CloseAudioDevice();
}

void set_track(track t)
{
	switch(t)
	{
		case TRACK_1: current = &track_1; break;
		case TRACK_2: current = &track_2; break;
		case TRACK_3: current = &track_3; break;
		default: current = nullptr; break;
	}
	if (!current) return;
	SeekMusicStream(*current, 0);
	PlayMusicStream(*current);
}

void play_effect(track t)
{
	switch(t)
	{
		case FX_INTRO: PlaySound(fx_intro); break;
		case FX_WIN: PlaySound(fx_win); break;
		default: break;
	}
	StopMusicStream(*current);
}
void music_main_loop()
{
// #ifdef DEBUG
    if (current) UpdateMusicStream(*current);
// #endif
}
