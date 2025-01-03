#include "menu.h"
#include "msg.h"
#include "rect.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace std
{
		
}

struct animation
{
    std::vector<Vector2> points;

    void print()
    {
        for (const Vector2& v : points)
        {
            msg(MSG_INFO, "point: %.2f %.2f", v.x, v.y);
        }
    }

    void add(float time, float value)
    {
        size_t min = 0, max = points.size();
        size_t current = 0;

        while (min < max)
        {
            current = (min + max) / 2;

            if (points[current].x == time)
            {
                throw std::runtime_error("Point at time " + std::to_string(time) + " already exists");
            }
            else if (points[current].x < time)
            {
                min = current + 1;
            }
            else
            {
                max = current;
            }
        }
        points.insert(points.begin() + min, Vector2{time, value});
    }

    float get(float time)
    {
        if (points.empty())
        {
            throw std::runtime_error("No points in the animation");
        }

        if (time <= points.front().x)
        {
            return points.front().y;
        }

        if (time >= points.back().x)
        {
            return points.back().y;
        }

        size_t i = 1;
        while (i < points.size() && points[i].x < time)
        {
            i++;
        }

        const Vector2& a = points[i - 1];
        const Vector2& b = points[i];

        float m = (b.y - a.y) / (b.x - a.x);
        return m * (time - a.x) + a.y;
    }
};


#define TEXT(txt, x_pos, y_pos, sz, color) {\
	Vector2 text_sz = MeasureTextEx(rend.font, txt, sz, 0); 	\
	Vector2 loc = {x_pos - text_sz.x/2, y_pos - text_sz.y/2};	\
	DrawTextEx	\
	(	\
		rend.font,	\
		txt,	\
		loc,	\
		sz,		\
		0,		\
		color);}

void draw_menu(renderer& rend)
{
	float min_sz = std::min(rend.width, rend.height);
	float unit = min_sz / 10;
	float font_sz_large = min_sz / 10;
	float font_sz_small = min_sz / 20;
	rect menu_rect = rect
	(
		(rend.width - min_sz) / 2 + min_sz / 20,
		(rend.height - min_sz) / 2 + min_sz / 20,
		min_sz - min_sz / 10,
		min_sz - min_sz / 10
	);
	Color fg = 0xefeffeff_color;
	DrawRectangleRounded(menu_rect, 0.1, 60, 0x1a1b26ff_color);
	DrawRectangleRoundedLinesEx(menu_rect, 0.1, 60, 5, fg);
	float vloc = unit;
	TEXT("How to play:", rend.width/2, vloc, font_sz_large, fg);
	vloc += unit * 1;
	TEXT("You must deliver a parcel to each", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("drop off point (the white tile).", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("You can place a parcel on a pressure", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("pad to open its corresponding door.", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("Conveyor belts move objects across", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("them every turn", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 1;
	TEXT("Controls:", rend.width/2, vloc, font_sz_large, fg);
	vloc += unit * 1;
	TEXT("WASD: Movement", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("Space: Skip Turn", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("R: Restart Level", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("Esc: This Menu", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.8f;
	TEXT("Enjoy!", rend.width/2, vloc, font_sz_large, fg);
 
}


bool play_intro(renderer& rend)
{
	static float begin_time = GetTime();
	float anim = GetTime() - begin_time;
	float min_sz = std::min(rend.width, rend.height);
	float unit = min_sz / 10;
	float font_sz_large = min_sz / 10;
	float font_sz_small = min_sz / 20;
	rect menu_rect = rect
	(
		(rend.width - min_sz) / 2 + min_sz / 20,
		(rend.height - min_sz) / 2 + min_sz / 20,
		min_sz - min_sz / 10,
		min_sz - min_sz / 10
	);
	float vloc = unit;

	Color fg = 0xefeffeff_color;
	Color tint = WHITE;	
	animation opacity;
	animation img_opacity;
	
	opacity.add(0, 0);
	opacity.add(2, 1);
	opacity.add(7, 1);
	opacity.add(9, 0);

	img_opacity.add(3, 0);
	img_opacity.add(4, 1);
	img_opacity.add(7, 1);
	img_opacity.add(9, 0);
	
	fg.a = 0xff * opacity.get(anim);
	tint.a = 0xff * img_opacity.get(anim);
	TEXT("From the creators of", rend.width/2, vloc, font_sz_large, fg);
	vloc += unit * 1;
	TEXT("Reginald", rend.width/2, vloc, font_sz_large, fg);
	vloc += unit * 1;

	rect dest(
		rend.width/2 - unit * 4,
		rend.height/2 - unit * 2,
		unit * 8,
		unit * 8
	);
	rect splash_res(0, 0, rend.splash.width, rend.splash.height);
	DrawTexturePro(rend.splash, splash_res, dest, Vector2{0, 0}, 0, tint);

	if (anim <= 9) return false;

	UnloadTexture(rend.splash);
	return true;
}
