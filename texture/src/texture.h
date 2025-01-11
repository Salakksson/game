#ifndef TILESET_H_
#define TILESET_H_

#include <raylib.h>
#include <stdexcept>
#include <unordered_map>
#include <cstdint>
#include "coord.h"
#include "rect.h"

enum tile_id
{
	BLOCK_NONE = 0,
	OBJECT_NONE = 0,
	BLOCK_WALL,
	BLOCK_DOOR,
	BLOCK_PAD,
	BLOCK_FINISH,
	BLOCK_CONVEYOR,
	BLOCK_CONVEYOR_SAFE,
	OBJECT_START = 128,
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

enum tflags : uint16_t
{
	TEXTURE_IS_SET 		= 1,
	TEXTURE_CONNECTED 	= 2,
	TEXTURE_ANIMATED 	= 4,
	TEXTURE_OBJECT 		= 8,
};

struct __attribute__ ((packed)) 
tloc 
{
	uint16_t id;
	uint16_t x, y;
	uint16_t size;
};

struct __attribute__ ((packed)) 
texture
{
	tflags flags;
	size_t n_textures;
	tloc locs[];
};

struct __attribute__ ((packed)) 
pixel
{
	uint8_t 
	r, g, b, a;
};

struct __attribute__ ((packed)) 
tileset
{
	uint16_t width, height;
	pixel tex[];
	
	tileset();
};

#endif
