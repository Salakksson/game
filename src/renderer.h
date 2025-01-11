#ifndef RENDERER_H_
#define RENDERER_H_

#include <raylib.h>
#include "target.h"
#include "operators.h"

struct renderer
{
    float width;
    float height;
    
	Color color_bg = 0x1a1b26ff_color;
	
	target<float> zoom = 0.16f;

    Camera2D cam;
	Texture2D splash;
    Font font;
	bool cam_enabled;
	struct {
		int fps;
		int count;
		double last_second;
	}fps;

    void begin();
    void end();
    void end_cam();

    void draw_fps();

    renderer(int x, int y);
    ~renderer();

    void update();
};


bool IsWindowResizedWasm();


#endif
