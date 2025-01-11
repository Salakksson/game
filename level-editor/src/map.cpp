#include "map.h"
void map::load()
{
}

void map::draw()
{
	for (const auto& [loc, t] : grid)
	{
		atlas.draw(t, loc);
	}
}

void map::place(int x, int y, tile t)
{
	grid[coord(x, y)] = t;
}

void map::set_door(uint8_t type, bool open)
{
	uint8_t color = type % PAD_INVERSE;
	for(auto& [loc, t] : grid)
	{
		if (t.block != BLOCK_DOOR) continue;
		if (t.block_type % DOOR_CLOSED == color) 
		{
			if (open) 
				t.block_type &= ~DOOR_CLOSED;
			else 
				t.block_type |= DOOR_CLOSED;
		}
	}
}

void map::update()
{
    for (auto &entry : grid)
    {
        const coord location = entry.first;
        tile &current_tile = entry.second;
		
		if (!atlas.rects[current_tile.block].is_connected) continue;

        auto check_neighbor = [&](int dx, int dy, direction dir) 
        {
            coord neighbor_pos(location.x + dx, location.y + dy);
            auto it = grid.find(neighbor_pos);
            
            if (it != grid.end() && it->second.block == current_tile.block)
            {
                current_tile.block_type |= dir;
            }
        };

        check_neighbor(-1, 0, DIR_LEFT);
        check_neighbor(1, 0, DIR_RIGHT);
        check_neighbor(0, -1, DIR_UP);
        check_neighbor(0, 1, DIR_DOWN);
    }
}

// void map::set_cam(Camera2D& cam)
// {
// 	float rect_aspect = boundary.w / boundary.h;
// 	float screen_width = GetScreenWidth();
// 	float screen_height = GetScreenHeight();
// 	float screen_aspect = screen_width / screen_height;
//
// 	float zoom = (screen_aspect > rect_aspect) ?
// 		screen_height / boundary.h :
// 		screen_width / boundary.w;
//
// 	cam.zoom = zoom;
//
// 	cam.target = Vector2{boundary.x + boundary.w / 2.0f, boundary.y + boundary.h / 2.0f};
// 	cam.offset = Vector2{screen_width / 2.0f, screen_height/ 2.0f};
// }


