#pragma once
#include <memory>
#include "Utilities.h"
#include <string>
#include <sol/sol.hpp>

class FileLoader
{
private:

public:
	FileLoader();
	~FileLoader();

	void LoadProject(sol::state& lua, const std::string& filename, const std::unique_ptr<class AssetManager> assetManager, std::unique_ptr<struct SDL_Renderer*, Util::SDLDestroyer> renderer);
	void LoadMap(const std::unique_ptr<class AssetManager>& assetManager, std::unique_ptr<struct SDL_Renderer*, Util::SDLDestroyer> renderer, const std::string& filename);
};