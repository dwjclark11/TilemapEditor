#pragma once
#include <memory>
#include "Utilities.h"
#include <string>
#include <sol/sol.hpp>
#include <filesystem>
#include "../AssetManager.h"

class FileLoader
{
private:
	void LoadMap(const AssetManager_Ptr& assetManager, const std::string& filename);
	void SaveMap(std::filesystem::path filename);
	void SaveColliders(std::filesystem::path filename);

public:
	FileLoader();
	~FileLoader();

	void LoadProject(sol::state& lua, const std::string& filename, const AssetManager_Ptr& assetManager,
		Renderer& renderer, std::vector<std::string>& assetIds, std::vector<std::string>& assetFilepaths,
		int& canvasWidth, int& canvasHeight, int& tileSize);

	void SaveProject(const std::string& filename, std::vector<std::string>& assetIds, std::vector<std::string>& assetFilepaths,
		const int& canvasWidth, const int& canvasHeight, const int& tileSize);
};