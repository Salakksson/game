#ifndef TILESET_H_
#define TILESET_H_

#include "coord.h"
#include "msg.h"
#include "rect.h"
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <raylib.h>

enum eblock: uint8_t
{
	BLOCK_NONE = 0,
	OBJECT_NONE = 0,
	BLOCK_WALL,
	BLOCK_DOOR,
	BLOCK_PAD,
	BLOCK_FINISH,
	BLOCK_CONVEYOR,
	BLOCK_CONVEYOR_SAFE,
	OBJECT_MIN,
	OBJECT_PARCEL,
	OBJECT_PARCEL_MINI,
	OBJECT_PLAYER,
};
enum direction : uint8_t
{
	DIR_NONE 	= 0,
	DIR_LEFT 	= 1,
	DIR_RIGHT 	= 2,
	DIR_UP	 	= 4,
	DIR_DOWN 	= 8,
};
enum : uint8_t
{
	DOOR_GREEN		= 0,
	DOOR_BLUE		= 1,
	DOOR_RED 		= 2,
	DOOR_YELLOW 	= 3,

	CONVEYOR_RIGHT	= 0,
	CONVEYOR_LEFT	= 4,
	CONVEYOR_UP		= 8,
	CONVEYOR_DOWN	= 12,

	PAD_INVERSE 	= 4,
	DOOR_OPEN 	  	= 0,
	DOOR_CLOSED 	= 8,
};

struct tile
{
	eblock block = BLOCK_NONE;
	eblock object = OBJECT_NONE;
	uint8_t block_type = 0;
	uint8_t obj_type = 0;
};

struct texture
{
	bool is_connected = false;
	rect list[16] = {};
	bool is_set[16] = {false};
	int fps = 0, frames = 1;
	inline rect get(size_t index)
	{
		if (index >= 16) throw std::out_of_range("Index out of bounds in texture::get");
		return (is_set[index]) ? list[index] : list[DIR_NONE];
	}
	inline void set(size_t index, rect r)
	{
		if (index >= 16) throw std::out_of_range("Index out of bounds in texture::set");
		list[index] = r;
		is_set[index] = true;
	}
	inline void set(size_t index, coord c)
	{
		if (index >= 16) throw std::out_of_range("Index out of bounds in texture::set");
		set(index, rect(16 * c.x, 16 * c.y, 16, 16));
	}
	inline void set(coord c, int fps, int frames)
	{
		for(int i = 0; i < frames; i++)
		{
			set(i, c + coord(i, 0));
		}
	}
	inline int get_frame()
	{
		return fps ? int(fps * GetTime()) % frames : 0;
	}
	texture(int, int, int, int);
	inline texture(){};
};


struct tileset
{
	std::unordered_map<eblock, texture> rects;
	Texture2D tex;
	
	inline void draw(tile block, coord c, float size)
	{
		int frame = rects[block.block].get_frame();
		if (block.block)
			DrawTexturePro(tex, 
				rects[block.block].get(block.block_type + frame),
				rect(c.x + (1 - size)/2, c.y - (1 - size)/2, size, size), 
				Vector2{0, 0}, 
				0, WHITE);
		bool adjust = (block.object == OBJECT_PLAYER || block.object == OBJECT_PARCEL || block.object == OBJECT_PARCEL_MINI);
		if (block.object)
			DrawTexturePro(tex, 
				rects[block.object].get(block.obj_type),
				rect((float)c.x + 1.0f/32.0f * adjust - (1 - size)/2, c.y - (1 - size)/2, size, size), 
				Vector2{0, 0}, 
				0, WHITE);
	}
	tileset();
};

inline bool is_solid(tile t)
{
	switch(t.block)
	{
		case BLOCK_WALL: return true;
		case BLOCK_DOOR: return t.block_type & DOOR_CLOSED;
		default: return false;
	}
}
inline bool is_pushable(tile t)
{
	if (is_solid(t)) return false;
	switch (t.object)
	{
		case OBJECT_PARCEL: return true;
		case OBJECT_PARCEL_MINI: return true;
		default: return false;
	}
}

#endif
