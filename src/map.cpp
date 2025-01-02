#include "map.h"

#include "rect.h"
#include "msg.h"

#include <cmath>
#include <fstream>

inline bool is_object(eblock t)
{
	return t > OBJECT_MIN;
}
tile get_block(char c)
{
	eblock value;
	switch(c)
	{
		case 'W': value = BLOCK_WALL; break;
		case 'D': value = BLOCK_DOOR; break;
		case 'P': value = BLOCK_PAD; break;
		case 'F': value = BLOCK_FINISH; break;
		case 'g': value = OBJECT_PLAYER; break;
		case 'p': value = OBJECT_PARCEL; break;
		default: value = BLOCK_NONE; break;
	}
	tile t;
	if (!is_object(value))
		t.block = value;
	else t.object = value;
	return t;
}

uint8_t get_type(char c)
{
	switch(c)
	{
		case 'g': return DOOR_GREEN;
		case 'b': return DOOR_BLUE;
		case 'r': return DOOR_RED;
		case 'y': return DOOR_YELLOW;
		default: return 0;
	}
}

void map::init()
{
	std::ifstream file(path);	
	
	std::string line;
	int x_min = std::numeric_limits<int>::max();
	int x_max = std::numeric_limits<int>::min();
	int y_min = std::numeric_limits<int>::max();
	int y_max = std::numeric_limits<int>::min();
	int y = 0;
	while(std::getline(file, line))
	{
		int x = 0;
		bool subcol = 0;
		for(char c : line)
		{
			coord tile = coord(x, y);
			if (!subcol)
			{
				grid[tile] = get_block(c);
				if (grid[tile].block)
				{
					if (x < x_min) x_min = x; 
					if (y < y_min) y_min = y; 
					if (x > x_max) x_max = x; 
					if (y > y_max) y_max = y; 
				}
				if(grid[tile].object == OBJECT_PLAYER) grid[tile].obj_type = DIR_DOWN; 
			}
			else
			{
				switch(grid[tile].block)
				{
					case BLOCK_DOOR: grid[tile].block_type |= DOOR_CLOSED;
					break;
					default: break;
				}
				grid[tile].block_type |= get_type(c);
				x++;
			}
			subcol = !subcol;	
		}
		y++;
	}
	this->update();
	this->boundary = rect(x_min - 0.5, y_min - 0.5, x_max - x_min + 2, y_max - y_min + 2);
}

void map::draw()
{
	for (const auto& [coord, t] : grid)
	{
		atlas.draw(t, coord, anim);
	}
}

void map::place(int x, int y, eblock b)
{
	grid[coord(x, y)].block = b;
}
void map::place(int x, int y, tile t)
{
	grid[coord(x, y)] = t;
}

void map::set_door(uint8_t color, bool open)
{
	for(auto& [loc, t] : grid)
	{
		if (t.block != BLOCK_DOOR) continue;
		if (t.block_type % DOOR_CLOSED == color) 
			if (open) 
				t.block_type &= ~DOOR_CLOSED;
			else 
				t.block_type |= DOOR_CLOSED;
	}
}

bool map::move(coord a, coord mv)
{
	msg(MSG_INFO, "movement: %i, %i: %i, %i", a.x, a.y, mv.x, mv.y);
	tile block_a = grid[a];
	msg(MSG_INFO, "block: %i, %i: %i, %i", block_a.block, block_a.block_type, block_a.object, block_a.obj_type);
	tile block_b = grid[a + mv];
	if(is_solid(block_b)) return false;

	if(is_pushable(block_b)) if (!move(a + mv, mv)) return false;
	grid[a + mv].object = block_a.object;
	grid[a + mv].obj_type = block_a.obj_type;
	grid[a].object = OBJECT_NONE;
	if (grid[a+mv].block == BLOCK_PAD) 
	{
		msg(MSG_INFO, "setting door new (%i) to %i:", grid[a+mv].block, grid[a+mv].object == OBJECT_PARCEL);
		set_door(grid[a+mv].block_type % DOOR_CLOSED, (grid[a+mv].object == OBJECT_PARCEL));
	}
	if (grid[a].block == BLOCK_PAD) 
	{
		msg(MSG_INFO, "setting door old (%i) to %i:", grid[a].block, grid[a].object == OBJECT_PARCEL);
		set_door(grid[a].block_type % DOOR_CLOSED, (grid[a].object == OBJECT_PARCEL));
	}
	msg(MSG_INFO, "block: %i, %i: %i, %i", block_b.block, block_b.block_type, block_b.object, block_b.obj_type);
	// msg(MSG_INFO, "a: %i %i: %i %i, b: %i %i: %i %i", a.x, a.y, block_a.block, block_a.type, (a+mv).x, (a+mv).y, block_b.block, block_b.type);
	return true;
}
bool map::move(coord a, direction dir)
{
	coord mv;
	switch(dir)
	{
		case DIR_LEFT: mv = coord(-1, 0); break;
		case DIR_RIGHT: mv = coord(-1, 0); break;
		case DIR_UP: mv = coord(-1, 0); break;
		case DIR_DOWN: mv = coord(-1, 0); break;
		case DIR_NONE: return false;
	}
	return move(a, mv);
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

void map::set_cam(Camera2D& cam)
{
	float rect_aspect = boundary.w / boundary.h;
	float screen_width = GetScreenWidth();
	float screen_height = GetScreenHeight();
	float screen_aspect = screen_width / screen_height;

	float zoom = (screen_aspect > rect_aspect) ?
		screen_height / boundary.h :
		screen_width / boundary.w;

	cam.zoom = zoom;

	cam.target = Vector2{boundary.x + boundary.w / 2.0f, boundary.y + boundary.h / 2.0f};
	cam.offset = Vector2{screen_width / 2.0f, screen_height/ 2.0f};
}

state map::check_state()
{
	int n_players = 0;
	int n_finish = 0;
	int n_finish_full = 0;
	int n_parcels = 0;
	for (const auto& [loc, t] : grid)
	{
		if (t.block == BLOCK_FINISH)
		{
			n_finish++;
			if (t.object == OBJECT_PARCEL)
				n_finish_full++;
		}
		if (t.object == OBJECT_PARCEL)
			n_parcels++;
		if (t.object == OBJECT_PLAYER)
			n_players++;
	}
		// msg(MSG_INFO, "p: %d, f: %d, ff: %d, p: %d", n_players, n_finish, n_finish_full, n_parcels);
		if (n_players < 1) return STATE_UNWINNABLE;
		if (n_parcels < n_finish) return STATE_UNWINNABLE;
		if (n_finish == n_finish_full) return STATE_WON;
		return STATE_PLAYING;
}
