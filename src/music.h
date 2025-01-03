#ifndef MUSIC_H_
#define MUSIC_H_

#define MUSIC_DIR "assets/music/"
enum track
{
	TRACK_1,
	TRACK_2,
	TRACK_3,
	FX_WIN,	
	FX_INTRO,	
};

void music_init();
void music_quit();
void music_main_loop();
void set_track(track);
void play_effect(track);

#endif
