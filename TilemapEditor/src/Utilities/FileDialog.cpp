#include "Utilities/FileDialog.h"
#include "Logger/Logger.h"

#include <nfd.hpp>

std::string FileDialog::OpenFile()
{
	NFD::UniquePath outPath;
	nfdfilteritem_t filterItems[ 2 ] = { { "Lua Files", "lua" }, { "Tilemap Files", "map" } };

	auto result = NFD::OpenDialog( outPath, filterItems, 2 );
	if ( result == NFD_OKAY )
	{
		return std::string{ outPath.get() };
	}
	else if ( result == NFD_CANCEL )
	{
		return std::string{ "" };
	}

	LOG_ERROR( "Failed to open dialog: {}", NFD::GetError() );

	return std::string{ "" };
}

std::string FileDialog::SaveFile()
{
	NFD::UniquePath outPath;
	nfdfilteritem_t filterItems[ 2 ] = { { "Lua Files", "lua" }, { "Tilemap Files", "map" } };
	auto result = NFD::SaveDialog( outPath, filterItems, 2 );
	if ( result == NFD_OKAY )
	{
		return std::string{ outPath.get() };
	}
	else if ( result == NFD_CANCEL )
	{
		return std::string{ "" };
	}

	LOG_ERROR( "Failed to open dialog: {}", NFD::GetError() );

	return std::string{ "" };
}

std::string FileDialog::OpenImageFile()
{
	NFD::UniquePath outPath;
	nfdfilteritem_t filterItems[ 3 ] = {
		{ "PNG Files", "png" }, { "Bitmap Files", "bmp" }, { "JPEG Files", "jpg,jpeg" } };
	auto result = NFD::OpenDialog( outPath, filterItems, 3 );
	if ( result == NFD_OKAY )
	{
		return std::string{ outPath.get() };
	}
	else if ( result == NFD_CANCEL )
	{
		return std::string{ "" };
	}

	LOG_ERROR( "Failed to open dialog: {}", NFD::GetError() );

	return std::string{ "" };
}
