#ifndef RENDERER_H_
#define RENDERER_H_

#include <raylib.h>
#include "target.h"

struct renderer
{
    float width;
    float height;
    
	Color color_bg = GetColor(0x1a1b26ff);
	
	target<float> zoom = 0.16f;

    Camera2D cam;

    Font font;
	bool cam_enabled = false;

    void begin();
    void end();
    void end_cam();
    void begin_cam();

    renderer(int x, int y);
    ~renderer();

    void update();
};


bool IsWindowResizedWasm();


#endif
