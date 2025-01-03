#include <cmath>
#define MSG_H_IMPL
#include "msg.h"
#include "renderer.h"
#include "music.h"
#include "tileset.h"
#include "map.h"
#include "menu.h"

Sound door1;
Sound door2;
Sound door3;
Sound step1;
Sound step2;

bool g_quit = 0;
// #define PLATFORM_WEB
#ifndef PLATFORM_WEB
int main()
{
	renderer rend(800, 600);
	
	msg(MSG_INFO, "info");
	tileset atlas;
	
	music_init();
	door1 = LoadSound(ASSETS_DIR"music/door1.wav");
	door2 = LoadSound(ASSETS_DIR"music/door2.wav");
	door3 = LoadSound(ASSETS_DIR"music/door3.wav");
	step1 = LoadSound(ASSETS_DIR"music/step1.wav");
	step2 = LoadSound(ASSETS_DIR"music/step2.wav");

	map tutorial(ASSETS_DIR"maps/tutorial.map", atlas);	
	map level_1(ASSETS_DIR"maps/level_1.map", atlas);	
	map level_2(ASSETS_DIR"maps/level_2.map", atlas);	
	map level_3(ASSETS_DIR"maps/level_3.map", atlas);	
	map level_4(ASSETS_DIR"maps/level_4.map", atlas);	
	map level_5(ASSETS_DIR"maps/level_5.map", atlas);	
	map level_6(ASSETS_DIR"maps/level_6.map", atlas);	
	map level_7(ASSETS_DIR"maps/level_7.map", atlas);	

	map levels[] = {
		tutorial,
		level_1,
		level_2,
		level_3,
		level_4,
		level_5,
		level_6,
		level_7,
	};
	int level_number = 0;
	map current_level = levels[level_number];
	current_level.set_cam(rend.cam);
	
	state game_state = STATE_INTRO;
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
			current_level.set_cam(rend.cam);
			
			if (IsKeyPressed(KEY_SPACE)) current_level.sim_conveyors();
			coord mv = {
				IsKeyPressed(KEY_D) - IsKeyPressed(KEY_A),
				IsKeyPressed(KEY_S) - IsKeyPressed(KEY_W)
			};
			for (auto& [c, t] : current_level.grid)
			{
				if (t.object != OBJECT_PLAYER) continue;
				// msg(MSG_INFO, "found player");
				if (mv.x > 0) t.obj_type = DIR_RIGHT;
				if (mv.x < 0) t.obj_type = DIR_LEFT;
				if (mv.y > 0) t.obj_type = DIR_DOWN;
				if (mv.y < 0) t.obj_type = DIR_UP;
				if (mv.x | mv.y) 
				{
					current_level.move(c, mv);
					current_level.sim_conveyors();
				}
				break;
			}
		};
		
		float mouse_x = GetMouseX();
		float mouse_y = GetMouseY();

		rend.begin();

		switch (game_state)
		{
		case STATE_INTRO:
            rend.end_cam();
			if (play_intro(rend)) game_state = STATE_MENU;
			break;
		case STATE_MENU:
			draw_game();
            rend.end_cam();
            draw_menu(rend);
			if (IsKeyPressed(KEY_ESCAPE)) game_state = current_level.check_state();
			break;
		case STATE_LOST:
			draw_game();
			msg(MSG_FATAL, "fix the code! lost");
			if (IsKeyPressed(KEY_ESCAPE)) game_state = STATE_MENU;
			break;
		case STATE_UNWINNABLE:
			draw_game();
			msg(MSG_FATAL, "fix the code! unwinnable");
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
			current_level.anim -= GetFrameTime();
			if (current_level.anim >= 0) break;
			current_level = levels[++level_number];
			game_state = STATE_PLAYING;
			break;
		default: msg(MSG_FATAL, "fix the code!");
		}
		rend.end();
	}
}

#else // Web main function

#include <emscripten/emscripten.h>
	renderer rend(800, 600);
	
	tileset atlas;
	map start(ASSETS_DIR"maps/start.map", atlas);	
	player p(rend.cam, start, atlas);
	

	texture heart(atlas, coord(0, 5));

void main_loop()
{
	// UNIMPLEMENTED("web assembly main function");
		game_main_loop();
		float mouse_x = GetMouseX();
		float mouse_y = GetMouseY();
		
		Vector2 mouse_pos = GetScreenToWorld2D({mouse_x, mouse_y}, rend.cam);
		coord mouse_coord(std::floor(mouse_pos.x), std::floor(mouse_pos.y));	

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			start.place(mouse_coord.x, mouse_coord.y, BLOCK_WALL);
			start.update();
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
		{
			start.place(mouse_coord.x, mouse_coord.y, BLOCK_NONE);
			start.update();
		}

		rend.begin();

		start.draw();
		p.update();
		p.draw();
		
		heart.draw(rect(0, 0, 1, 1));
		rend.end();
}

int main()
{
	msg(MSG_INFO, "Running web mode");
	music_init();
	game_init();
	heart.animated = true;
	heart.fps = 4;
	heart.frames = 4;
	emscripten_set_main_loop(main_loop, 300, 0);
}

#endif
