#pragma once
#include <string>

/*
*  This is intended to be a cross platform filedialog.
*  Currently, I only have it set up to be used on a windows machine and uses the functions from
*  FileDialogWin.h/.cpp; however, you can create your own filedialog classes to use for your specific OS
*  ex: FileDialogMac.h/.cpp and it should have the same functionality and should not have to be changed
*  anywhere else in the code. I Believe.....
*
*/

class FileDialog
{
public:
	/*
	*  	Returns the path in the form of an std::string of the file we want to open
	*	so we can use out open file functions
	*/
	std::string OpenFile();

	/*
	*  	Returns the path in the form of an std::string of the file we want to save to
	*/
	std::string SaveFile();

	/*
	*   Returns the path to the image we want to open in the editor
	*/
	std::string OpenImageFile();
};