#include "menu.h"
#include "msg.h"
#include "rect.h"
#include <algorithm>
#include <stdexcept>
#include <vector>
#include "music.h"

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
	bool ended(float time)
	{
        if (points.empty())
        {
            throw std::runtime_error("No points in the animation");
        }
		return (points.back().x < time);
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
	float unit = min_sz / 11;
	float font_sz_large = min_sz / 12;
	float font_sz_small = min_sz / 24;
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
	float vloc = unit + (rend.height - min_sz)/ 2;
	TEXT("How to play:", rend.width/2, vloc, font_sz_large, fg);
	vloc += unit * 0.7;
	TEXT("You must deliver a parcel to each drop", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("off point (the white tile). You can place", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("a parcel on a pressure pad to open its", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("corresponding door. Hollow parcels don't", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("activate pressure pads. conveyor belts", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("move objects forward one block every turn.", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.7;
	TEXT("Controls:", rend.width/2, vloc, font_sz_large, fg);
	vloc += unit * 0.7;
	TEXT("WASD: Movement", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("Space: Skip Turn", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("Z: Undo move", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("R: Restart Level", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("Esc: This Menu", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.7f;
	TEXT("Enjoy!", rend.width/2, vloc, font_sz_large, fg);
	
	vloc += unit * 1.0f;
	rect music_rect = {
		rend.width/2 - unit * 2,
		vloc,
		unit * 6,
		unit / 4
	};
	rect music_rect_bar = {
		rend.width/2 - unit * 2,
		vloc,
		unit * 6 * music_volume,
		unit / 4
	};
	DrawTextEx(rend.font, "Music: ", Vector2{rend.width/2 - unit * 4, vloc - font_sz_small/4}, font_sz_small, 0, fg);
	vloc += unit * 0.5f;
	rect sound_rect = {
		rend.width/2 - unit * 2,
		vloc,
		unit * 6,
		unit / 4
	};
	rect sound_rect_bar = {
		rend.width/2 - unit * 2,
		vloc,
		unit * 6 * sound_volume,
		unit / 4
	};
	DrawTextEx(rend.font, "Sound: ", Vector2{rend.width/2 - unit * 4, vloc - font_sz_small/4}, font_sz_small, 0, fg);
	DrawRectangleRounded(music_rect, 0.1, 20, 0x1a1b26ff_color);
	DrawRectangleRounded(music_rect_bar, 0.1, 20, 0xff9966ff_color);
	DrawRectangleRoundedLinesEx(music_rect, 0.1, 20, 1, fg);
	DrawRectangleRounded(sound_rect, 0.1, 20, 0x1a1b26ff_color);
	DrawRectangleRounded(sound_rect_bar, 0.1, 20, 0xff9966ff_color);
	DrawRectangleRoundedLinesEx(sound_rect, 0.1, 20, 1, fg);

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 mouse_pos = GetMousePosition();
		if (CheckCollisionPointRec(mouse_pos, music_rect))
		{
			music_volume = (mouse_pos.x - music_rect.x)/music_rect.w;
			msg(MSG_INFO, "Mouse down (button 1) %f %f", mouse_pos.x, mouse_pos.y);
		}
		if (CheckCollisionPointRec(mouse_pos, sound_rect))
		{
			sound_volume = (mouse_pos.x - sound_rect.x)/sound_rect.w;
			msg(MSG_INFO, "Mouse down (button 2) %f %f", mouse_pos.x, mouse_pos.y);
		}
		else msg(MSG_INFO, "Mouse down %f %f", mouse_pos.x, mouse_pos.y);
	}
 
}


bool play_intro(renderer& rend)
{
	static float begin_time = GetTime();
	float anim = GetTime() - begin_time;
	float min_sz = std::min(rend.width, rend.height);
	float unit = min_sz / 10;
	float font_sz_large = min_sz / 10;
	float vloc = unit + (rend.height - min_sz)/2;

	Color fg = 0xefeffeff_color;
	Color tint = WHITE;	
	animation opacity;
	animation img_opacity;
	
	opacity.add(0, 0);
	opacity.add(2, 1);
	opacity.add(5, 1);
	opacity.add(7, 0);

	img_opacity.add(1, 0);
	img_opacity.add(3, 1);
	img_opacity.add(5, 1);
	img_opacity.add(7, 0);
	
	fg.a = 0xff * opacity.get(anim);
	tint.a = 0xff * img_opacity.get(anim);
	TEXT("From the creators of", rend.width/2, vloc, font_sz_large, fg);
	vloc += unit * 1;
	TEXT("Reginald", rend.width/2, vloc, font_sz_large, fg);
	vloc += unit * 1;

	rect dest(
		rend.width/2 - unit * 3,
		rend.height/2 - unit * 1,
		unit * 6,
		unit * 6
	);
	rect splash_res(0, 0, rend.splash.width, rend.splash.height);
	DrawTexturePro(rend.splash, splash_res, dest, Vector2{0, 0}, 0, tint);

	if (!opacity.ended(anim)) return false;

	// UnloadTexture(rend.splash);
	return true;
}

void draw_menu_end(renderer& rend)
{
	float min_sz = std::min(rend.width, rend.height);
	float unit = min_sz / 11;
	float font_sz_large = min_sz / 12;
	float font_sz_small = min_sz / 24;
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
	float vloc = unit + (rend.height - min_sz)/2;
	TEXT("Thanks for playing!", rend.width/2, vloc, font_sz_large, fg);
	vloc += unit * 0.7;
	TEXT("I was hoping to add more levels but i", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("ran out of time. ):", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("Hope you enjoyed the game, i would", rend.width/2, vloc, font_sz_small, fg);
	vloc += unit * 0.5f;
	TEXT("like some feedback if possible", rend.width/2, vloc, font_sz_small, fg);
	
	vloc = unit * 9.3f;
	rect music_rect = {
		rend.width/2 - unit * 2,
		vloc,
		unit * 6,
		unit / 4
	};
	rect music_rect_bar = {
		rend.width/2 - unit * 2,
		vloc,
		unit * 6 * music_volume,
		unit / 4
	};
	DrawTextEx(rend.font, "Music: ", Vector2{rend.width/2 - unit * 4, vloc - font_sz_small/4}, font_sz_small, 0, fg);
	vloc += unit * 0.5f;
	rect sound_rect = {
		rend.width/2 - unit * 2,
		vloc,
		unit * 6,
		unit / 4
	};
	rect sound_rect_bar = {
		rend.width/2 - unit * 2,
		vloc,
		unit * 6 * sound_volume,
		unit / 4
	};
	DrawTextEx(rend.font, "Sound: ", Vector2{rend.width/2 - unit * 4, vloc - font_sz_small/4}, font_sz_small, 0, fg);
	DrawRectangleRounded(music_rect, 0.1, 20, 0x1a1b26ff_color);
	DrawRectangleRounded(music_rect_bar, 0.1, 20, 0xff9966ff_color);
	DrawRectangleRoundedLinesEx(music_rect, 0.1, 20, 1, fg);
	DrawRectangleRounded(sound_rect, 0.1, 20, 0x1a1b26ff_color);
	DrawRectangleRounded(sound_rect_bar, 0.1, 20, 0xff9966ff_color);
	DrawRectangleRoundedLinesEx(sound_rect, 0.1, 20, 1, fg);

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 mouse_pos = GetMousePosition();
		if (CheckCollisionPointRec(mouse_pos, music_rect))
		{
			music_volume = (mouse_pos.x - music_rect.x)/music_rect.w;
			msg(MSG_INFO, "Mouse down (button 1) %f %f", mouse_pos.x, mouse_pos.y);
		}
		if (CheckCollisionPointRec(mouse_pos, sound_rect))
		{
			sound_volume = (mouse_pos.x - sound_rect.x)/sound_rect.w;
			msg(MSG_INFO, "Mouse down (button 2) %f %f", mouse_pos.x, mouse_pos.y);
		}
		else msg(MSG_INFO, "Mouse down %f %f", mouse_pos.x, mouse_pos.y);
	}
 
}
