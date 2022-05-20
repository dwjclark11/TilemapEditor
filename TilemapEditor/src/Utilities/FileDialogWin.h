#pragma once

#include <Windows.h>
#include <string>
/*
*	Windows platform implementation of the FileDialogs needed for loading and saving files on a windows machine
*/
class FileDialogWin
{
public:
	/*
	*  	OpenFile --> Opens a windows file dialog. By default, the filters are set to .lua and .map files; however, we
	*	only truly need to open the project file, which is a lua file. The project loader will load any map files for us.
	*	The function returns the path as an std::string of the file to open if successful, if not returns an empty string.
	*/
	static std::string OpenFile(const char* filter = "Lua Files (*.lua) |*.lua\0*.lua\0 Tilemap Files (*.map) | *.map\0*.map\0", HWND owner = NULL);

	/*
	*  	SaveFile --> Opens a windows file dialog.
	*	The function returns the path as an std::string of the file to save if successful, if not returns an empty string.
	*/
	static std::string SaveFile(const char* filter = "Lua Files (*.lua) |*.lua\0*.lua\0 Tilemap Files (*.map) | *.map\0*.map\0", HWND owner = NULL);


	/*
	*  	OpenImageFile--> Opens a windows file dialog.
	*	The function returns the path as an std::string of the image we want to use if successful, if not returns an empty string.
	*/
	static std::string OpenImageFile(const char* filter = "PNG Files (*.png) | *.png\0*.png\0 Bitmap Files (*.bmp) |*.bmp\0*.bmp\0 JPEG Files (*.jpg) |*.jpg\0*.jpg\0", HWND owner = NULL);
};
