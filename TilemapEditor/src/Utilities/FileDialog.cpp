#include "FileDialog.h"

#ifdef _WIN32
	#include "FileDialogWin.h"
#elif
	// Add Include here
#endif

std::string FileDialog::OpenFile()
{
	#ifdef _WIN32
		return FileDialogWin::OpenFile();
	#elif
		// Add your OS specific Dialog Here
	#endif
}

std::string FileDialog::SaveFile()
{
	#ifdef _WIN32
		return FileDialogWin::SaveFile();
	#elif
		// Add your OS specific Dialog Here
	#endif
}

std::string FileDialog::OpenImageFile()
{
	#ifdef _WIN32
		return FileDialogWin::OpenImageFile();
	#elif
		// Add your OS specific Dialog Here
	#endif
}
