#include "music.h"

static Music* current;
static Sound* queued_sound;

static Sound fx_intro;
static Sound fx_win;
static Music track_1;
static Music track_2;
static Music track_3;
static Music track_4;
static Music track_5;
static Music track_6;
static Music track_7;

float music_volume = 0.5f;
float sound_volume = 0.5f;

void music_init()
{
    InitAudioDevice();

	track_1 = LoadMusicStream(MUSIC_DIR"tracks/music-1.wav");
	track_2 = LoadMusicStream(MUSIC_DIR"tracks/music-2.wav");
	track_3 = LoadMusicStream(MUSIC_DIR"tracks/music-3.wav");
	track_4 = LoadMusicStream(MUSIC_DIR"tracks/music-4.wav");
	track_5 = LoadMusicStream(MUSIC_DIR"tracks/music-5.wav");
	track_6 = LoadMusicStream(MUSIC_DIR"tracks/music-6.wav");
	track_7 = LoadMusicStream(MUSIC_DIR"tracks/music-7.wav");

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
		case TRACK_4: current = &track_4; break;
		case TRACK_5: current = &track_5; break;
		case TRACK_6: current = &track_6; break;
		case TRACK_7: current = &track_7; break;
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
		case FX_INTRO: queue_sound(&fx_intro); break;
		case FX_WIN: queue_sound(&fx_win); break;
		default: break;
	}
	StopMusicStream(*current);
}
void music_main_loop()
{
	SetMusicVolume(*current, music_volume);
    if (current) UpdateMusicStream(*current);
	if (queued_sound)
	{
		SetSoundVolume(*queued_sound, sound_volume);
		PlaySound(*queued_sound);
		queued_sound = nullptr;
	}
}

void queue_sound(Sound* s)
{
	queued_sound = s;
}
