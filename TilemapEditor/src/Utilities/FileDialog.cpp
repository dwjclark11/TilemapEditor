#include "FileDialog.h"

#ifdef _WIN32
#include "FileDialogWin.h"
#elif defined TARGET_OS_MAC
// Add mac include file Here
#elif defined __linux__
// Add linux include file Here
#endif

std::string FileDialog::OpenFile()
{
#ifdef _WIN32
	return FileDialogWin::OpenFile();
#elif defined TARGET_OS_MAC
	// Add your OS specific Dialog Here
#elif defined __linux__
	// Add your OS specific Dialog Here
#endif
}

std::string FileDialog::SaveFile()
{
#ifdef _WIN32
	return FileDialogWin::SaveFile();
#elif defined TARGET_OS_MAC
	// Add your OS specific Dialog Here
#elif defined __linux__
	// Add your OS specific Dialog Here
#endif
}

std::string FileDialog::OpenImageFile()
{
#ifdef _WIN32
	return FileDialogWin::OpenImageFile();
#elif defined TARGET_OS_MAC
	// Add your OS specific Dialog Here
#elif defined __linux__
	// Add your OS specific Dialog Here
#endif
}
