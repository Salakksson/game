#include "editor.h"
#include "map.h"
#include "renderer.h"
#include "tileset.h"

int main()
{
	renderer rend(800, 600);
	tileset atlas;

	editor ed(rend, atlas);

	while (!WindowShouldClose())
	{
		rend.begin();
		
		ed.draw();

		rend.end();
	}
}
