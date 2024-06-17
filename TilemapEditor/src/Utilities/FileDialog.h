#pragma once
#include <string>

class FileDialog
{
  public:
	/*
	 *  Returns the path in the form of an std::string of the file we want to open
	 *	so we can use out open file functions
	 */
	std::string OpenFile();

	/*
	 *  Returns the path in the form of an std::string of the file we want to save to
	 */
	std::string SaveFile();

	/*
	 *  Returns the path to the image we want to open in the editor
	 */
	std::string OpenImageFile();
};