#ifndef COORD_H_
#define COORD_H_

#include <cstddef>
#include <raylib.h>

struct coord
{
	int x, y;
	inline coord(int x, int y): x(x), y(y) {}
	inline coord(): x(0), y(0) {}
	inline explicit coord(Vector2 v): x(v.x), y(v.y) {}
	inline explicit operator Vector2() 
	{
		return Vector2
		{
			(float)this->x, (float)this->y
		};
	}
};

inline bool operator==(const coord a, const coord b)
{
	return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const coord a, const coord b)
{
	return a.x != b.x || a.y != b.y;
}

inline coord operator+(const coord a, const coord b)
{
	return coord(a.x + b.x, a.y + b.y);
}

inline coord operator-(const coord a, const coord b)
{
	return coord(a.x - b.x, a.y - b.y);
}
#endif
