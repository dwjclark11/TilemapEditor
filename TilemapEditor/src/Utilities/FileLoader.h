#pragma once
#include <memory>
#include "Utilities.h"
#include <string>
#include <sol/sol.hpp>
#include <filesystem>

class FileLoader
{
private:
	void LoadMap(const std::unique_ptr<class AssetManager>& assetManager, const std::string& filename);
	void SaveMap(std::filesystem::path filename);

public:
	FileLoader();
	~FileLoader();

	void LoadProject(sol::state& lua, const std::string& filename, const std::unique_ptr<class AssetManager>& assetManager,
		std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, std::vector<std::string>& assetIds, std::vector<std::string>& assetFilepaths,
		int& canvasWidth, int& canvasHeight, int& tileSize);

	void SaveProject(const std::string& filename, std::vector<std::string>& assetIds, std::vector<std::string>& assetFilepaths, int& canvasWidth, int& canvasHeight, int& tileSize);
};