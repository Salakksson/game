#include "pch.h"
#include "map.h"
#include "std/fstream.h"

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
		default: return coord(0, 0);
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

tile get_type(tile t, char c)
{
	switch (t.block)
	{
		case BLOCK_DOOR:
		case BLOCK_PAD:
			switch(c)
			{
				case 'g': t.block_type = DOOR_GREEN; break;
				case 'b': t.block_type = DOOR_BLUE; break;
				case 'r': t.block_type = DOOR_RED; break;
				case 'y': t.block_type = DOOR_YELLOW; break;
				case 'G': t.block_type = DOOR_GREEN | PAD_INVERSE; break;
				case 'B': t.block_type = DOOR_BLUE | PAD_INVERSE; break;
				case 'R': t.block_type = DOOR_RED | PAD_INVERSE; break;
				case 'Y': t.block_type = DOOR_YELLOW | PAD_INVERSE; break;
				default: break;
			}
			break;
		case BLOCK_CONVEYOR:
			if (c == 's')
			{
				t.block = BLOCK_CONVEYOR_SAFE;
			}
		default: break;
	}
	return t;
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
				grid[tile] = get_type(grid[tile], c);
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
		if (t.block != BLOCK_PAD) continue;
		if (t.block_type & PAD_INVERSE)
		{
			bool pushed = (t.object != OBJECT_NONE);
			set_door(t.block_type, !pushed);
		}
		else
		{
			bool pushed = (t.object != OBJECT_NONE);
			set_door(t.block_type, pushed);
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
		{
			if (open) 
				t.block_type &= ~DOOR_CLOSED;
			else 
				t.block_type |= DOOR_CLOSED;
		}
	}
}

bool map::can_move(coord a, coord mv)
{
	tile block_b = grid[a + mv];
	
	if(is_solid(block_b)) return false;
	if(block_b.block == BLOCK_CONVEYOR_SAFE && grid[a].object == OBJECT_PLAYER) return false;
	if(is_pushable(block_b)) if (!can_move(a + mv, mv)) return false;
	return true;
}

static auto type_to_mv = [](uint8_t type)
{
	return 
		(type == CONVEYOR_RIGHT) ? coord(1, 0):
		(type == CONVEYOR_LEFT) ? coord(-1, 0):
		(type == CONVEYOR_UP) ? coord(0, -1):
		(type == CONVEYOR_DOWN) ? coord(0, 1): 
								coord(0, 0);
};

// bool check_recursion()
// {
//     void *current_return_address = __builtin_return_address(0);
//
//     void *caller_return_address = __builtin_return_address(1);
//
// 	return (current_return_address == caller_return_address);
// }

bool map::can_conveyor_move(coord pos)
{
	static std::vector<coord> previous;
	tile a = grid[pos];
	coord mv = type_to_mv(a.block_type);
	tile b = grid[pos + mv];
	if(is_solid(b)) return false;
	if(!b.object) return true;
	if(b.block == BLOCK_CONVEYOR || b.block == BLOCK_CONVEYOR_SAFE) return can_conveyor_move(pos + mv);
	if(is_pushable(b)) return can_move(pos+mv, mv);
	
	return true;
}

void map::sim_conveyors()
{
	struct move
	{
		coord start, end;
		tile t;
		bool push;
	};
	std::vector<move> moves;

	for (const auto& [loc, t] : grid)
	{
		if(t.block != BLOCK_CONVEYOR && t.block != BLOCK_CONVEYOR_SAFE) continue;
		if(t.object == OBJECT_NONE) continue;
		
		coord mv = type_to_mv(t.block_type);
		if (mv == coord(0, 0)) FATAL("shits fucked", 0);
		
		if (can_conveyor_move(loc))
		{
			bool push = (grid[loc + mv].block != BLOCK_CONVEYOR &&
						 is_pushable(grid[loc + mv]));
			moves.push_back({loc, loc + mv, t, push});
		}
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
			set_door(grid[m.end].block_type, open);
			if(open) PlaySound(door1);
		}
	}

}

bool map::move(coord a, coord mv)
{
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
		queue_sound(&door1);
	}
	if (grid[a].block == BLOCK_PAD) 
	{
		bool open = (grid[a].object != OBJECT_NONE && grid[a].object != OBJECT_PARCEL_MINI);
		open ^= (bool)(grid[a].block_type & PAD_INVERSE);
		set_door(grid[a].block_type % DOOR_CLOSED, open);
		queue_sound(&door2);
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
	if (n_players < 1) return STATE_UNWINNABLE;
	if (n_parcels < n_finish) return STATE_UNWINNABLE;
	if (n_finish == n_finish_full) return STATE_WON;
	return STATE_PLAYING;
}


void map::make_move(direction dir)
{
	Sound* sound = (rand() % 2) ? &step1 : &step2;
	queue_sound(sound);

	moves.push_back(dir);
	if (dir == DIR_NONE) return sim_conveyors();	
	for (auto& [c, t] : grid)
	{
		if (t.object != OBJECT_PLAYER) continue;
		if (grid[c + get_mv(dir)].block != BLOCK_CONVEYOR_SAFE)
			move(c, get_mv(dir));
		sim_conveyors();
		break;
	}
}

void map::reset()
{
	grid = original->grid;
	atlas = original->atlas;
	anim = original->anim;
	path = original->path;
	boundary = original->boundary;
	music = original->music;
	moves.clear();
}

void map::undo()
{
	std::vector<direction> new_moves = moves;
	new_moves.pop_back();
	reset();
	for (direction d : new_moves)
	{
		make_move(d);
	}
}

