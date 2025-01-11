#ifndef RECT_H_
#define RECT_H_

#include <raylib.h>

struct rect
{
	float x, y, w, h;
	inline rect(float x, float y, float w, float h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}
	inline rect(Vector2 min, Vector2 max)
	{
		x = min.x;
		y = min.y;
		w = max.x - min.x;
		h = max.y - min.y;
	}
	inline rect(): x(0), y(0), w(0), h(0) {}
	inline rect(Rectangle r)
	{
		x = r.x;
		y = r.y;
		w = r.width;
		h = r.height;
	}

	inline bool in_bound(Vector2 pos)
	{
		return 	(x <= pos.x  && pos.x <= x + w) &&
				(y <= pos.y  && pos.y <= y + h);
	}
	inline operator Rectangle() const 
	{
        return (Rectangle){.x = x, .y = y, .width = w, .height = h};
    }
	inline rect& operator+=(Vector2 v)
	{
		this->x += v.x;
		this->y += v.y;
		return *this;
	}
};

inline rect operator+(rect r, Vector2 v)
{
	return rect(r.x + v.x, r.y + v.y, r.w, r.h);
}

#endif
