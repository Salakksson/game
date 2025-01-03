#ifndef MAP_H_
#define MAP_H_

#include <unordered_map>
#include <vector>
#include "tileset.h"
#include "music.h"
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

enum state
{
	STATE_INTRO,
	STATE_MENU,
	STATE_LOST,
	STATE_UNWINNABLE,
	STATE_PLAYING,
	STATE_WON,
	STATE_END,
};

struct map
{
	const map* original = nullptr;
	tile_plane grid;
	tileset& atlas;
	
	float anim = 1.0f;
	const char* path;
	track music;	
	std::vector<direction> moves;

	rect boundary;
	void place(int x, int y, eblock b);
	void place(int x, int y, tile t);
	bool move(coord a, direction dir);
	bool move(coord a, coord mv);
	void make_move(direction dir);
	void draw();
	void update();
	void set_cam(Camera2D& cam);
	void set_door(uint8_t, bool);
	void init();
	bool can_move(coord a, coord mv);
	bool can_conveyor_move(coord pos);

	void sim_conveyors();
	void reset();
	void undo();

	state check_state();

	map(const char* filepath, tileset& textures) : atlas(textures), path(filepath)
	{
		init();
	}

	map(const map& other)
        : grid(other.grid),
          atlas(other.atlas),
          anim(other.anim),
          path(other.path),
		  music(other.music),
          boundary(other.boundary)
    {
		original = &other;
		moves = std::vector<direction>(30);
    }
	map& operator=(const map& other)
    {
		grid = other.grid;
		atlas = other.atlas;
		anim = other.anim;
		path = other.path;
		boundary = other.boundary;
		music = other.music;
		original = &other;
		moves = std::vector<direction>(30);
        return *this;
    }
};



#endif
