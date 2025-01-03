#ifndef MUSIC_H_
#define MUSIC_H_

#include <raylib.h>
#define MUSIC_DIR "assets/music/"
enum track
{
	TRACK_1,
	TRACK_2,
	TRACK_3,
	TRACK_4,
	TRACK_5,
	TRACK_6,
	TRACK_7,
	FX_WIN,	
	FX_INTRO,	
};

extern float music_volume;
extern float sound_volume;

void music_init();
void music_quit();
void music_main_loop();
void set_track(track);
void play_effect(track);
void queue_sound(Sound* s);

#endif
