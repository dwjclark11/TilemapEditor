#pragma once
#include <map>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include "./Utilities/Utilities.h"

class AssetManager
{
private:
	std::map<std::string, Texture> mTextures;

public:

	AssetManager();
	~AssetManager();

	// Add Textures
	void AddTexture(Renderer& renderer, const std::string& assetId, const std::string& filePath);
	const Texture& GetTexture(const std::string& assetId);
	bool HasTexture(const std::string& assetId);
	void RemoveTexture(const std::string& assetId);

};

typedef std::unique_ptr<AssetManager> AssetManager_Ptr;