#ifndef EDITOR_H_
#define EDITOR_H_

#include "map.h"
#include "rect.h"
#include "renderer.h"
#include "tileset.h"

std::string file_popup();

struct editor
{
	rect tile_display;
	tile current_tile;
	
	bool draw_grid;
	
	map level;
	renderer& rend;
	tileset& atlas;

	enum {
		READING,
		WRITING,
		EDITING,
	} state = READING;

	void save_map(const char* path);
	void load_map(const char* path);
	void handle_inputs();
	void draw();
	editor(renderer& rend, tileset& atlas): rend(rend), atlas(atlas), level(atlas) {}
};



#endif
