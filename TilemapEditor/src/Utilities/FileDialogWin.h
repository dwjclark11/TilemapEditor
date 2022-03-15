#pragma once
#include <Windows.h>
#include <string>

class FileDialog
{
public:
	std::string OpenFile(const char* filter = "Tilemap Files (*.map) | *.map\0*.map\0 Lua Files (*.lua) |*.lua\0*.lua\0", HWND owner = NULL);
	std::string SaveFile(const char* filter = "Tilemap Files (*.map) | *.map\0*.map\0 Lua Files (*.lua) |*.lua\0*.lua\0", HWND owner = NULL);

	std::string OpenImageFile(const char* filter = "PNG Files (*.png) | *.png\0*.png\0 Bitmap Files (*.bmp) |*.bmp\0*.bmp\0 JPEG Files (*.jpg) |*.jpg\0*.jpg\0", HWND owner = NULL);
};