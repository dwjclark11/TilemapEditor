#pragma once
#include <string>

class FileDialog
{
public:
	std::string OpenFile();
	std::string SaveFile();
	std::string OpenImageFile();
};