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
	Image img = LoadImage(ASSETS_DIR"atlas.png");
	if (!img.data) msg(MSG_FATAL, "Failed to load image");

	tex = LoadTextureFromImage(img);
	SetTextureFilter(tex, TEXTURE_FILTER_POINT);

	rects[BLOCK_NONE].set(0, rect(0, 0, 0, 0));
	rects[BLOCK_WALL] = texture(0, 0, 16, 16);

	rects[BLOCK_PAD].set(DOOR_GREEN, rect(128, 0, 16, 16)); 
	rects[BLOCK_PAD].set(DOOR_BLUE, rect(128, 16, 16, 16)); 
	rects[BLOCK_PAD].set(DOOR_RED, rect(128, 32, 16, 16)); 
	rects[BLOCK_PAD].set(DOOR_YELLOW, rect(128, 48, 16, 16)); 
	
	rects[BLOCK_DOOR].set(DOOR_GREEN | DOOR_CLOSED, rect(144, 0, 16, 16)); 
	rects[BLOCK_DOOR].set(DOOR_BLUE | DOOR_CLOSED, rect(144, 16, 16, 16)); 
	rects[BLOCK_DOOR].set(DOOR_RED | DOOR_CLOSED, rect(144, 32, 16, 16)); 
	rects[BLOCK_DOOR].set(DOOR_YELLOW | DOOR_CLOSED, rect(144, 48, 16, 16)); 

	rects[BLOCK_DOOR].set(DOOR_GREEN | DOOR_OPEN, rect(160, 0, 16, 16)); 
	rects[BLOCK_DOOR].set(DOOR_BLUE | DOOR_OPEN, rect(160, 16, 16, 16)); 
	rects[BLOCK_DOOR].set(DOOR_RED | DOOR_OPEN, rect(160, 32, 16, 16)); 
	rects[BLOCK_DOOR].set(DOOR_YELLOW | DOOR_OPEN, rect(160, 48, 16, 16)); 

	rects[BLOCK_FINISH].set(0, coord(5, 0)); 
	
	rects[OBJECT_PARCEL].set(0, coord(4, 0));

	rects[OBJECT_PLAYER].set(DIR_LEFT, coord(0, 6));
	rects[OBJECT_PLAYER].set(DIR_DOWN, coord(1, 6));
	rects[OBJECT_PLAYER].set(DIR_RIGHT, coord(2, 6));
	rects[OBJECT_PLAYER].set(DIR_UP, coord(3, 6));

	UnloadImage(img);
}
