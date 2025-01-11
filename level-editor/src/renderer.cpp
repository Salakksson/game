#include "renderer.h"
#include <rlgl.h>
#include <raymath.h>

renderer::renderer(int x, int y)
{
    width = x;
    height = y;
	
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    
    SetTargetFPS(0);

    InitWindow(width, height, "Game");
    SetExitKey(KEY_NULL);
    
    font = LoadFontEx("../assets/fonts/scp.ttf", 100, nullptr, 0); 
	
    cam = {0};
    cam.zoom = height * zoom.current;
    cam.target = Vector2{0, 0};
    cam.offset = Vector2{width / 2.0f, height / 2.0f};
}

void renderer::begin()
{
    this->update();

    BeginDrawing();
	begin_cam();
    ClearBackground(color_bg);

	rlPushMatrix();
	rlTranslatef(0, 25, 0);
	rlRotatef(90, 1, 0, 0);
	DrawGrid(100, 1);
	rlPopMatrix();
	DrawCircle(0, 0, 0.1, WHITE); 

}

void renderer::end_cam()
{
	if (cam_enabled)
	{
		EndMode2D();
		cam_enabled = false;
	}
}
void renderer::begin_cam()
{
	if (!cam_enabled)
	{
		BeginMode2D(this->cam);
		cam_enabled = true;
	}
}
void renderer::end()
{
	end_cam();

	EndDrawing();
}

void renderer::update()
{
	width = GetScreenWidth();
	height = GetScreenHeight();

	float wheel_delta = GetMouseWheelMove();
	zoom.wish *= pow(1.2, wheel_delta);
	cam.zoom = height * (float)zoom.update(GetFrameTime() * 5);
	cam.offset = Vector2{width / 2.0f, height / 2.0f};

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 mouse_delta = GetMouseDelta();
		Vector2 game_delta = GetScreenToWorld2D(mouse_delta, cam) - GetScreenToWorld2D({0, 0}, cam);
		cam.target -= game_delta;
	}
}

renderer::~renderer()
{
	CloseWindow();
}
