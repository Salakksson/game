#include "editor.h"
#include "msg.h"

#include <string>
#include <dirent.h>
#include <cstdlib>

#define MIN(a, b) ((a < b) ? a : b)

void insert_sorted(std::vector<std::string> &vec, const std::string &value)
{
    auto it = std::lower_bound(vec.begin(), vec.end(), value);

    vec.insert(it, value);
}

bool file_popup(renderer& rend, std::string* ret)
{
	rend.end_cam();
	static std::string path = realpath(".", nullptr);
	static float text_size = 1;
	static int idx = 0;
	DIR* dir = opendir(path.c_str());
	if (!dir)
	{
		FATAL("failed to read file '%s'", path.c_str());
	}
	dirent* entry;

	Color fg = GetColor(0xe0eeefff);
	float sz = MIN(rend.width, rend.height);
	float fontsize = text_size * sz/20;

	rect window((rend.width - sz)/2 + 10, (rend.height - sz)/2 + 10, sz - 20, sz - 20);
	DrawRectangleRec(window, GetColor(0x1a1b26ff));
	DrawRectangleLinesEx(window, 3, fg);

	std::vector<std::string> files;
	while ((entry = readdir(dir)))
	{
		std::string filename = entry->d_name;
		if (filename != ".")
			insert_sorted(files, filename);
	}
	
 	if (IsKeyPressed(KEY_UP))
    {
        idx = (idx - 1 + files.size()) % files.size();
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        idx = (idx + 1) % files.size();
    }

	float vloc = window.y;
	int current_idx = 0;
	
	Vector2 pos = {window.x + fontsize, vloc += fontsize};
	DrawTextEx(rend.font, path.c_str(), pos, fontsize, 0, fg);

	for (std::string filename : files)
	{
		Vector2 pos = {window.x + fontsize, vloc += fontsize};
		Color color = (idx == current_idx) ? GetColor(0xff9966ff) : fg;
		DrawTextEx(rend.font, filename.c_str(), pos, fontsize, 0, color);
		current_idx++;
	}
	rend.begin_cam();
	if (IsKeyPressed(KEY_ENTER))
	{
		std::string newpath = path + "/" + files[idx];
		if (files[idx] == "..")
		{
			size_t last_slash = path.find_last_of('/');
            path = (last_slash != std::string::npos) ? path.substr(0, last_slash) : ".";
			msg(MSG_INFO, "going into dir '%s'", path.c_str());
			closedir(dir);
			return false;
		}
		else if (opendir(newpath.c_str()))
		{
            path = newpath; 
			closedir(dir);
			return false;
		}
		*ret = newpath;
		path = ".";
		text_size = 1;
		idx = 0;
		closedir(dir);
		return true;
	}
	
	if (IsKeyDown(KEY_LEFT_SHIFT))
	{
		if (IsKeyPressed(KEY_MINUS))
			text_size *= 0.9;
		else if (IsKeyPressed(KEY_EQUAL))
			text_size /= 0.9;
	}
	closedir(dir);
	return false;
}


void editor::draw()
{
	switch (state)
	{
		case READING:
		{
			std::string filepath;
			if (file_popup(rend, &filepath))
			{
				msg(MSG_INFO, "selected file: %s", filepath.c_str());
			}
		}
		break;
		case WRITING:
		{
			std::string filepath;
			if (file_popup(rend, &filepath))
			{
				msg(MSG_INFO, "selected file: %s", filepath.c_str());
			}
		}
		break;
		case EDITING:
		{

		}
		break;
	}
}
