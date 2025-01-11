#ifndef MAP_H_
#define MAP_H_

#include <vector>
#include "tileset.h"
#include "coord.h"

struct coord_hasher
{
    template <typename T>
    size_t operator()(const T& coord) const
    {
        size_t h1 = std::hash<int>{}(coord.x);
        size_t h2 = std::hash<int>{}(coord.y);
        return h1 ^ (h2 << 1);
    }
};
typedef std::unordered_map<coord, tile, coord_hasher> tile_plane;

struct coord_tile
{
	coord loc;
	tile t;
};

struct mapfile
{
	size_t sz;
	coord_tile tiles[];
};

struct map
{
	tile_plane grid;
	tileset& atlas;
	
	const char* path;

	rect boundary;

	void place(int x, int y, tile t);
	void draw();
	void update();
	void set_door(uint8_t, bool);
	void load();

	map(tileset& textures) : atlas(textures) {}
};



#endif
