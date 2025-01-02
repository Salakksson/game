#ifndef RENDERER_H_
#define RENDERER_H_

#include "config.h"
#include "operators.h"
#include "target.h"

struct renderer
{
    float width;
    float height;
    
	Color color_bg = 0x1a1b26ff_color;
	
	target<float> zoom = 0.16f;

    Camera2D cam;
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
