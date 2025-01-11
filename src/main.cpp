#include "pch.h"
#include "music.h"
#include "menu.h"
#include "tileset.h"
#include "map.h"

Sound door1;
Sound door2;
Sound door3;
Sound step1;
Sound step2;

#define MAP(track, number) \
	map level_##number(ASSETS_DIR"maps/level_" #number ".map", atlas); \
	level_##number .music = track; n_levels++\

bool g_quit = 0;
#define PLATFORM_WEB
#ifdef PLATFORM_WEB
int main()
{
	renderer rend(800, 600);
	tileset atlas;
	
	music_init();
	door1 = LoadSound(MUSIC_DIR"fx/door1.wav");
	door2 = LoadSound(MUSIC_DIR"fx/door2.wav");
	door3 = LoadSound(MUSIC_DIR"fx/door3.wav");
	step1 = LoadSound(MUSIC_DIR"fx/step1.wav");
	step2 = LoadSound(MUSIC_DIR"fx/step2.wav");

	map tutorial(ASSETS_DIR"maps/tutorial.map", atlas);	
	tutorial.music = TRACK_1;
	
	int n_levels = 1;
	MAP(TRACK_1, 1);
	MAP(TRACK_1, 2);
	MAP(TRACK_2, 3);
	MAP(TRACK_2, 4);
	MAP(TRACK_2, 5);
	MAP(TRACK_2, 6);
	MAP(TRACK_3, 7);
	MAP(TRACK_4, 8);
	MAP(TRACK_4, 9);
	MAP(TRACK_5, 10);
	// MAP(TRACK_5, 11);

	map levels[] = {
		tutorial,
		level_1,
		level_2,
		level_3,
		level_4,
		level_5,
		level_6,
		level_7,
		level_8,
		level_9,
		level_10,
		// level_11,
	};
	int level_number = 0;
	map current_level = levels[level_number];
	set_track(current_level.music);	
	current_level.set_cam(rend.cam);
	
	state game_state = STATE_INTRO;
	play_effect(FX_INTRO);
	while(!WindowShouldClose())
	{
		music_main_loop();
		auto draw_game = [&]()
		{
			current_level.set_cam(rend.cam);
			current_level.draw();
		};
		auto play_game = [&]()
		{
			if (IsKeyPressed(KEY_R)) 
			{
				atlas = tileset();
				current_level = levels[level_number];
			}
			if (IsKeyPressed(KEY_Z)) 
			{
				current_level.undo();
			}
			if (IsKeyPressed(KEY_RIGHT))
			{
				if (level_number < n_levels - 1) current_level = levels[++level_number];
				set_track(current_level.music);	
			}
			if (IsKeyPressed(KEY_LEFT))
			{
				if (level_number > 0) current_level = levels[--level_number];
				set_track(current_level.music);	
			}
			if (IsKeyPressed(KEY_SPACE)) current_level.make_move(DIR_NONE);
			if (IsKeyPressed(KEY_W)) current_level.make_move(DIR_UP);
			if (IsKeyPressed(KEY_A)) current_level.make_move(DIR_LEFT);
			if (IsKeyPressed(KEY_S)) current_level.make_move(DIR_DOWN);
			if (IsKeyPressed(KEY_D)) current_level.make_move(DIR_RIGHT);
		};
		
		rend.begin();

		switch (game_state)
		{
		case STATE_INTRO:
            rend.end_cam();
			if (play_intro(rend)) 
			{
				game_state = STATE_MENU;
				set_track(current_level.music);	
			}
			break;
		case STATE_MENU:
			draw_game();
            rend.end_cam();
            draw_menu(rend);
			if (IsKeyPressed(KEY_ESCAPE)) game_state = current_level.check_state();
			break;
		case STATE_END:
            rend.end_cam();
            draw_menu_end(rend);
			break;
		case STATE_LOST:
		case STATE_UNWINNABLE:
			{
				draw_game();
				rend.end_cam();
				float unit = std::min(rend.width, rend.height)/10;
				DrawTextEx(rend.font, "This is an error in my code, press z to undo", {50, rend.height/2 - unit}, unit/2, 0, WHITE);
				if (IsKeyPressed(KEY_Z))
				{
					current_level.undo();
					game_state = current_level.check_state();
				}
				break;
			}
		case STATE_PLAYING:
			draw_game();
			play_game();
			rend.end_cam();
			DrawTextEx(rend.font, std::to_string(level_number).c_str(), Vector2{rend.width - 100, 20}, 50, 0, WHITE);
			game_state = current_level.check_state();
			if (IsKeyPressed(KEY_ESCAPE)) 
			{
				msg(MSG_INFO, "pressed escape");
				game_state = STATE_MENU;
			}
			break;
		case STATE_WON:
			draw_game();
			if (current_level.anim == 1) play_effect(FX_WIN);
			current_level.anim -= GetFrameTime();
			if (current_level.anim >= 0) break;
			game_state = STATE_PLAYING;
			if (level_number < n_levels - 1) 
			{
				current_level = levels[++level_number];
				set_track(current_level.music);	
			}
			else game_state = STATE_END;
			break;
		default: msg(MSG_FATAL, "fix the code!");
		}

		rend.end();
	}
}

#else // Web main function

#include <emscripten/emscripten.h>
	// renderer rend(800, 600);
	// 
	// tileset atlas;
	
// void main_loop()
// {
// 	// UNIMPLEMENTED("web assembly main function");
// 		float mouse_x = GetMouseX();
// 		float mouse_y = GetMouseY();
// 		
// 		Vector2 mouse_pos = GetScreenToWorld2D({mouse_x, mouse_y}, rend.cam);
// 		coord mouse_coord(std::floor(mouse_pos.x), std::floor(mouse_pos.y));	
//
// 		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
// 		{
// 			start.place(mouse_coord.x, mouse_coord.y, BLOCK_WALL);
// 			start.update();
// 		}
// 		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
// 		{
// 			start.place(mouse_coord.x, mouse_coord.y, BLOCK_NONE);
// 			start.update();
// 		}
//
// 		rend.begin();
//
// 		start.draw();
// 		p.update();
// 		p.draw();
// 		
// 		heart.draw(rect(0, 0, 1, 1));
// 		rend.end();
// }

int main()
{
	msg(MSG_INFO, "Running web mode");

	renderer rend(800, 600);
	msg(MSG_INFO, "info");
	tileset atlas;
	
	music_init();
	door1 = LoadSound(MUSIC_DIR"fx/door1.wav");
	door2 = LoadSound(MUSIC_DIR"fx/door2.wav");
	door3 = LoadSound(MUSIC_DIR"fx/door3.wav");
	step1 = LoadSound(MUSIC_DIR"fx/step1.wav");
	step2 = LoadSound(MUSIC_DIR"fx/step2.wav");

	map tutorial(ASSETS_DIR"maps/tutorial.map", atlas);	
	tutorial.music = TRACK_1;
	map level_1(ASSETS_DIR"maps/level_1.map", atlas);	
	level_1.music = TRACK_1;
	map level_2(ASSETS_DIR"maps/level_2.map", atlas);	
	level_2.music = TRACK_2;
	map level_3(ASSETS_DIR"maps/level_3.map", atlas);	
	level_3.music = TRACK_2;
	map level_4(ASSETS_DIR"maps/level_4.map", atlas);	
	level_4.music = TRACK_3;
	map level_5(ASSETS_DIR"maps/level_5.map", atlas);	
	level_5.music = TRACK_3;
	map level_6(ASSETS_DIR"maps/level_6.map", atlas);	
	level_6.music = TRACK_3;
	map level_7(ASSETS_DIR"maps/level_7.map", atlas);	
	level_7.music = TRACK_3;
	map level_8(ASSETS_DIR"maps/level_8.map", atlas);	
	level_8.music = TRACK_3;
	map level_9(ASSETS_DIR"maps/level_9.map", atlas);	
	level_9.music = TRACK_3;

	map levels[] = {
		tutorial,
		level_1,
		level_2,
		level_3,
		level_4,
		level_5,
		level_6,
		level_7,
		level_8,
		level_9,
	};
	int level_number = 0;
	map current_level = levels[level_number];
	set_track(current_level.music);	
	current_level.set_cam(rend.cam);
	
	state game_state = STATE_PLAYING;
	play_effect(FX_INTRO);
	while(!WindowShouldClose())
	{
		music_main_loop();
		auto draw_game = [&]()
		{
			current_level.draw();
		};
		auto play_game = [&]()
		{
			if (IsKeyPressed(KEY_R)) 
			{
				atlas = tileset();
				current_level = levels[level_number];
			}
			if (IsKeyPressed(KEY_Z)) 
			{
				current_level.undo();
			}
			current_level.set_cam(rend.cam);
		};
		
		if (IsKeyPressed(KEY_SPACE)) current_level.make_move(DIR_NONE);
		if (IsKeyPressed(KEY_W)) current_level.make_move(DIR_UP);
		if (IsKeyPressed(KEY_A)) current_level.make_move(DIR_LEFT);
		if (IsKeyPressed(KEY_S)) current_level.make_move(DIR_DOWN);
		if (IsKeyPressed(KEY_D)) current_level.make_move(DIR_RIGHT);

		float mouse_x = GetMouseX();
		float mouse_y = GetMouseY();

		rend.begin();

		switch (game_state)
		{
		case STATE_INTRO:
            rend.end_cam();
			if (play_intro(rend)) 
			{
				game_state = STATE_MENU;
				set_track(current_level.music);	
			}
			break;
		case STATE_MENU:
			draw_game();
            rend.end_cam();
            draw_menu(rend);
			if (IsKeyPressed(KEY_ESCAPE)) game_state = current_level.check_state();
			break;
		case STATE_LOST:
			draw_game();
			msg(MSG_ERROR, "fix the code! lost");
			if (IsKeyPressed(KEY_ESCAPE)) game_state = STATE_MENU;
			break;
		case STATE_UNWINNABLE:
			draw_game();
			msg(MSG_ERROR, "fix the code! unwinnable");
			if (IsKeyPressed(KEY_ESCAPE)) game_state = STATE_MENU;
			break;
		case STATE_PLAYING:
			draw_game();
			play_game();
			game_state = current_level.check_state();
			if (IsKeyPressed(KEY_ESCAPE)) 
			{
				msg(MSG_INFO, "pressed escape");
				game_state = STATE_MENU;
			}
			break;
		case STATE_WON:
			draw_game();
			if (current_level.anim == 1) play_effect(FX_WIN);
			current_level.anim -= GetFrameTime();
			if (current_level.anim >= 0) break;
			current_level = levels[++level_number];
			set_track(current_level.music);	
			game_state = STATE_PLAYING;
			break;
		default: msg(MSG_FATAL, "fix the code!");
		}
		rend.end();
	}
	// music_init();
	// game_init();
	// heart.animated = true;
	// heart.fps = 4;
	// heart.frames = 4;
	// emscripten_set_main_loop(main_loop, 300, 0);
}

#endif
