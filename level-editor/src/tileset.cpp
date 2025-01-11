#include "tileset.h"

#include "msg.h"

texture::texture(int x, int y, int width, int height)
{
// TODO: figure out why the fuck the debug prints some wierd ass numbers

	// msg(MSG_INFO, "width: %d, height: %d", width, height);
	is_connected = true;
	for (int i = 0; i < 16; i++)
	{
		int ix = i % 4;
		int iy = i / 4;
		rect r(
			width * (x + ix),
			height * (y + iy),
			width,
			height
		);
		// msg(MSG_INFO, "%d %d: %d %d %d %d", ix, iy, r.x, r.y, r.w, r.h);
		this->set(i, r);
	}
}

tileset::tileset()
{
	Image img = LoadImage("../assets/atlas.png");
	if (!img.data) msg(MSG_FATAL, "Failed to load image");

	tex = LoadTextureFromImage(img);
	SetTextureFilter(tex, TEXTURE_FILTER_POINT);

	rects[BLOCK_NONE].set(0, rect(0, 0, 0, 0));
	rects[BLOCK_WALL] = texture(0, 0, 16, 16);

	rects[BLOCK_PAD].set(DOOR_GREEN, coord(8, 0)); 
	rects[BLOCK_PAD].set(DOOR_BLUE, coord(8, 1)); 
	rects[BLOCK_PAD].set(DOOR_RED, coord(8, 2)); 
	rects[BLOCK_PAD].set(DOOR_YELLOW, coord(8, 3)); 

	rects[BLOCK_PAD].set(DOOR_GREEN | PAD_INVERSE, coord(9, 0)); 
	rects[BLOCK_PAD].set(DOOR_BLUE | PAD_INVERSE, coord(9, 1)); 
	rects[BLOCK_PAD].set(DOOR_RED | PAD_INVERSE, coord(9, 2)); 
	rects[BLOCK_PAD].set(DOOR_YELLOW | PAD_INVERSE, coord(9, 3)); 
	
	rects[BLOCK_DOOR].set(DOOR_GREEN | DOOR_CLOSED, coord(10, 0)); 
	rects[BLOCK_DOOR].set(DOOR_BLUE | DOOR_CLOSED, coord(10, 1)); 
	rects[BLOCK_DOOR].set(DOOR_RED | DOOR_CLOSED, coord(10, 2)); 
	rects[BLOCK_DOOR].set(DOOR_YELLOW | DOOR_CLOSED, coord(10, 3)); 

	rects[BLOCK_DOOR].set(DOOR_GREEN | DOOR_OPEN, coord(11, 0)); 
	rects[BLOCK_DOOR].set(DOOR_BLUE | DOOR_OPEN, coord(11, 1)); 
	rects[BLOCK_DOOR].set(DOOR_RED | DOOR_OPEN, coord(11, 2)); 
	rects[BLOCK_DOOR].set(DOOR_YELLOW | DOOR_OPEN, coord(11, 3));

	rects[BLOCK_CONVEYOR].fps = 5;
	rects[BLOCK_CONVEYOR].frames = 4;

	rects[BLOCK_CONVEYOR].set(CONVEYOR_RIGHT, coord(4, 4)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_RIGHT + 1, coord(5, 4)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_RIGHT + 2, coord(6, 4)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_RIGHT + 3, coord(7, 4)); 

	rects[BLOCK_CONVEYOR].set(CONVEYOR_LEFT, coord(4, 5)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_LEFT + 1, coord(5, 5)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_LEFT + 2, coord(6, 5)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_LEFT + 3, coord(7, 5)); 

	rects[BLOCK_CONVEYOR].set(CONVEYOR_UP, coord(4, 6)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_UP + 1, coord(5, 6)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_UP + 2, coord(6, 6)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_UP + 3, coord(7, 6)); 

	rects[BLOCK_CONVEYOR].set(CONVEYOR_DOWN, coord(4, 7)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_DOWN + 1, coord(5, 7)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_DOWN + 2, coord(6, 7)); 
	rects[BLOCK_CONVEYOR].set(CONVEYOR_DOWN + 3, coord(7, 7)); 

	rects[BLOCK_CONVEYOR_SAFE].fps = 5;
	rects[BLOCK_CONVEYOR_SAFE].frames = 4;

	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_RIGHT, coord(8, 4)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_RIGHT + 1, coord(9, 4)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_RIGHT + 2, coord(10, 4)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_RIGHT + 3, coord(11, 4)); 

	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_LEFT, coord(8, 5)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_LEFT + 1, coord(9, 5)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_LEFT + 2, coord(10, 5)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_LEFT + 3, coord(11, 5)); 

	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_UP, coord(8, 6)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_UP + 1, coord(9, 6)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_UP + 2, coord(10, 6)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_UP + 3, coord(11, 6)); 

	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_DOWN, coord(8, 7)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_DOWN + 1, coord(9, 7)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_DOWN + 2, coord(10, 7)); 
	rects[BLOCK_CONVEYOR_SAFE].set(CONVEYOR_DOWN + 3, coord(11, 7)); 

	rects[BLOCK_FINISH].set(0, coord(5, 0)); 
	
	rects[OBJECT_PARCEL].set(0, coord(4, 0));
	rects[OBJECT_PARCEL_MINI].set(0, coord(6, 0));

	rects[OBJECT_PLAYER].set(DIR_LEFT, coord(0, 6));
	rects[OBJECT_PLAYER].set(DIR_DOWN, coord(1, 6));
	rects[OBJECT_PLAYER].set(DIR_RIGHT, coord(2, 6));
	rects[OBJECT_PLAYER].set(DIR_UP, coord(3, 6));

	UnloadImage(img);
}
