#include "map.h"

#include "rect.h"
#include "msg.h"

#include <cmath>
#include <fstream>

extern Sound door1;
extern Sound door2;
extern Sound door3;
extern Sound step1;
extern Sound step2;

inline bool is_object(eblock t)
{
	return t > OBJECT_MIN;
}
coord get_mv(direction dir)
{
	coord mv;
	switch(dir)
	{
		case DIR_LEFT: return coord(-1, 0);
		case DIR_RIGHT: return coord(1, 0);
		case DIR_UP: return coord(-0, -1);
		case DIR_DOWN: return coord(0, 1);
		case DIR_NONE: return coord(0, 0);
	};
}
direction get_dir(coord mv)
{
	if (mv.x > 0) return DIR_RIGHT;
	if (mv.x < 0) return DIR_LEFT;
	if (mv.y > 0) return DIR_DOWN;
	if (mv.y < 0) return DIR_UP;
	else return DIR_NONE;
}

tile get_block(char c)
{
	eblock value;
	uint8_t type = 0;
	switch(c)
	{
		case 'W': value = BLOCK_WALL; break;
		case 'D': value = BLOCK_DOOR; break;
		case 'P': value = BLOCK_PAD; break;
		case 'F': value = BLOCK_FINISH; break;
		case 'g': value = OBJECT_PLAYER; break;
		case 'p': value = OBJECT_PARCEL; break;
		case 'm': value = OBJECT_PARCEL_MINI; break;
		case '>': value = BLOCK_CONVEYOR; type = CONVEYOR_RIGHT; break;
		case '<': value = BLOCK_CONVEYOR; type = CONVEYOR_LEFT; break;
		case '^': value = BLOCK_CONVEYOR; type = CONVEYOR_UP; break;
		case 'v': value = BLOCK_CONVEYOR; type = CONVEYOR_DOWN; break;
		default: value = BLOCK_NONE; break;
	}
	tile t;
	if (!is_object(value))
	{
		t.block = value;
		t.block_type = type;
	}
	else 
	{
		t.object = value;
		t.obj_type = type;
	}
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
		case 'G': return DOOR_GREEN | PAD_INVERSE;
		case 'B': return DOOR_BLUE | PAD_INVERSE;
		case 'R': return DOOR_RED | PAD_INVERSE;
		case 'Y': return DOOR_YELLOW | PAD_INVERSE;
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

	for (auto& [loc, t] : grid)
	{
		if (t.object != OBJECT_NONE && (t.obj_type & PAD_INVERSE))
		{
			set_door(t.block_type, true);
		}
	}
}

void map::draw()
{
	for (const auto& [loc, t] : grid)
	{
		atlas.draw(t, loc, anim);
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

void map::set_door(uint8_t type, bool open)
{
	uint8_t color = type % PAD_INVERSE;
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

bool map:: can_move(coord a, coord mv)
{
	tile block_b = grid[a + mv];
	
	if(is_solid(block_b)) return false;

	if(is_pushable(block_b)) if (!can_move(a + mv, mv)) return false;
	return true;
}
void map::sim_conveyors()
{
	auto type_to_mv = [](uint8_t type)
	{
		return 
			(type == CONVEYOR_RIGHT) ? coord(1, 0):
			(type == CONVEYOR_LEFT) ? coord(-1, 0):
			(type == CONVEYOR_UP) ? coord(0, -1):
			(type == CONVEYOR_DOWN) ? coord(0, 1): 
									coord(0, 0);
	};
	
	struct move
	{
		coord start, end;
		tile t;
		bool push;
	};
	std::vector<move> moves;

	for (const auto& [loc, t] : grid)
	{
		if(t.block != BLOCK_CONVEYOR) continue;
		if(t.object == OBJECT_NONE) continue;
	
		coord mv = type_to_mv(t.block_type);
		if (mv == coord(0, 0)) FATAL("shits fucked", 0);
		if (!can_move(loc, mv)) continue;
		if (is_pushable(grid[loc + mv]))
		{
			if (can_move(loc + mv, mv))
			{
				moves.push_back({loc, loc + mv, t, true});
			}
		}
		else
		moves.push_back({loc, loc + mv, t, false});
	}
	for (move m : moves)
	{
		if (m.push) this->move(m.start, m.end - m.start);
		bool replace_start = true;
		for (move m2 : moves)
		{
			if(m2.end != m.start) continue;
			replace_start = false;
			break;
		}
		grid[m.end].object = m.t.object;
		grid[m.end].obj_type = m.t.obj_type;
		if (replace_start)
		{
			grid[m.start].object = OBJECT_NONE;
			grid[m.start].obj_type = OBJECT_NONE;
		}
		if (grid[m.end].block == BLOCK_PAD) 
		{
			bool open = (grid[m.end].object != OBJECT_NONE && grid[m.end].object != OBJECT_PARCEL_MINI);
			open ^= (bool)(grid[m.end].block_type & PAD_INVERSE);
		// msg(MSG_INFO, "moving onto pad conveyor: (%d %d): %d %d: %d: xor: %d", (m.end).x, (m.end).y, grid[m.end].block_type, grid[m.end].object, open, (grid[m.end].block_type & PAD_INVERSE));
			set_door(grid[m.end].block_type, open);
			if(open) PlaySound(door1);
		}
	}

}

bool check_recursion()
{
    void *current_return_address = __builtin_return_address(0);

    void *caller_return_address = __builtin_return_address(1);

	return (current_return_address == caller_return_address);
}
bool map::move(coord a, coord mv)
{
	static Sound& sound = step1;

	bool is_first = check_recursion();
	if(!is_first) 
	{
		sound = (rand() % 2) ? step1 : step2;
		PlaySound(sound);
	}

	if (grid[a].object == OBJECT_PLAYER)
	{
		grid[a].obj_type = get_dir(mv);
	}

	tile block_a = grid[a];
	tile block_b = grid[a + mv];
	if(is_solid(block_b)) return false;
	
	if(is_pushable(block_b)) 
	{	
		if (!move(a + mv, mv)) return false;
	}
	grid[a + mv].object = block_a.object;
	grid[a + mv].obj_type = block_a.obj_type;
	grid[a].object = OBJECT_NONE;
	if (grid[a+mv].block == BLOCK_PAD) 
	{
		bool open = (grid[a+mv].object != OBJECT_NONE && grid[a+mv].object != OBJECT_PARCEL_MINI);
		open ^= (bool)(grid[a + mv].block_type & PAD_INVERSE);
		set_door(grid[a+mv].block_type, open);
		if(open) sound = door1;
	}
	if (grid[a].block == BLOCK_PAD) 
	{
		bool open = (grid[a].object != OBJECT_NONE && grid[a].object != OBJECT_PARCEL_MINI);
		open ^= (bool)(grid[a].block_type & PAD_INVERSE);
		set_door(grid[a].block_type % DOOR_CLOSED, open);
		if(open) sound = door2;
	}
	return true;
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


void map::make_move(direction dir)
{
	if (dir == DIR_NONE) return sim_conveyors();	
	for (auto& [c, t] : grid)
	{
		if (t.object != OBJECT_PLAYER) continue;
		move(c, get_mv(dir));
		sim_conveyors();
		break;
	}
}

