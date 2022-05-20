#include "FileDialogWin.h"

std::string FileDialogWin::OpenFile(const char* filter, HWND owner)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn))
	{
		return ofn.lpstrFile;
	}

	return std::string();
}

std::string FileDialogWin::SaveFile(const char* filter, HWND owner)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = owner;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = szFile;
	ofn.lpstrDefExt = "lua"; // If the user does not put the lua extension, add it 
	ofn.nMaxFile = sizeof(szFile);
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetSaveFileNameA(&ofn))
	{
		return ofn.lpstrFile;
	}

	return std::string();
}

std::string FileDialogWin::OpenImageFile(const char* filter, HWND owner)
{
	return OpenFile(filter, owner);
}
