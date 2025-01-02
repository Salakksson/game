#include "renderer.h"
#include <cstdio>
#include <rlgl.h>
#include <raymath.h>
#include "msg.h"
#include "rect.h"
#include <vector>

renderer::renderer(int x, int y)
{
    width = x;
    height = y;
	
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    
    SetTargetFPS(0);
#ifdef DEBUG
	SetTargetFPS(30000);
	this->fps = {.last_second = GetTime()};
#endif
    InitWindow(width, height, "Game");
    SetExitKey(KEY_F12);
    
    font = LoadFontEx(ASSETS_DIR"fonts/scp.ttf", 100, nullptr, 0); 
	
    cam = {0};
    cam.zoom = height * zoom.current;
    cam.target = Vector2{0, 0};
    cam.offset = Vector2{width / 2.0f, height / 2.0f};
}

void renderer::begin()
{
    this->update();

    BeginDrawing();
    BeginMode2D(this->cam);
    ClearBackground(color_bg);
}

void renderer::end_cam()
{
    EndMode2D();
    cam_enabled = false;
}
void renderer::end()
{
    if (cam_enabled) EndMode2D();
    cam_enabled = false;
#ifdef DEBUG
	double time = GetTime();
	if (time > 1 + fps.last_second)
	{
		fps.fps = fps.count;
		fps.count = 0;
		fps.last_second = time;
	}
	else
	{
		fps.count++;
	}
    draw_fps();
#endif
    EndDrawing();
}

void renderer::update()
{
}

void renderer::draw_fps()
{
    char buffer[20] = {0};
 	// float fps_width = MeasureTextEx(font, buffer, 50, 0).x;
    DrawTextEx(font, buffer, Vector2{20, 20}, 50, 0, WHITE);
	
	float input_sz = height/8;
	Vector2 input_pos = {width - 15 - 3 * input_sz, 15};

	rect inputs(input_pos.x, input_pos.y, input_sz * 3, input_sz * 2);
	rect w(input_pos.x + input_sz, input_pos.y, input_sz, input_sz);
	rect a(input_pos.x, input_pos.y + input_sz, input_sz, input_sz);
	rect s(input_pos.x + input_sz, input_pos.y + input_sz, input_sz, input_sz);
	rect d(input_pos.x + 2 * input_sz, input_pos.y + input_sz, input_sz, input_sz);
	// DrawRectangleLinesEx(inputs, 2, WHITE);
	DrawRectangleLinesEx(w, 2, IsKeyDown(KEY_W) ? RED : WHITE);
	DrawRectangleLinesEx(a, 2, IsKeyDown(KEY_A) ? RED : WHITE);
	DrawRectangleLinesEx(s, 2, IsKeyDown(KEY_S) ? RED : WHITE);
	DrawRectangleLinesEx(d, 2, IsKeyDown(KEY_D) ? RED : WHITE);
}

renderer::~renderer()
{
	CloseWindow();
}
