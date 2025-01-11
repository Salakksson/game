#include "pch.h"
#include <rlgl.h>
#include <raymath.h>

renderer::renderer(int x, int y)
{
    width = x;
    height = y;
	
    SetTraceLogLevel(LOG_WARNING);
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    
    SetTargetFPS(0);
#ifdef DEBUG
	SetTargetFPS(30000);
	this->fps = {.last_second = GetTime()};
#endif
    InitWindow(width, height, "Game");
    SetExitKey(KEY_NULL);
    
	// Image image = LoadImage(ASSETS_DIR"splash.png");
	// splash = LoadTextureFromImage(image);
	// UnloadImage(image);
	splash = LoadTexture(ASSETS_DIR"splash.png");

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
    cam_enabled = true;
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
	width = GetScreenWidth();
	height = GetScreenHeight();
}

void renderer::draw_fps()
{
    // char buffer[20] = {0};
    // sprintf(buffer, "FPS: %d", fps.fps);
    // DrawTextEx(font, buffer, Vector2{20, 20}, 50, 0, WHITE);
}

renderer::~renderer()
{
	CloseWindow();
}
