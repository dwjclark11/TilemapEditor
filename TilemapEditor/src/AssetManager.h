#pragma once
#include <map>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include "./Utilities/Utilities.h"

/*
*	AssetManager Class. In the asset manager only has to take care of textures in this editor.
*/
class AssetManager
{
private:
	std::map<std::string, Texture> mTextures;

public:

	AssetManager();
	~AssetManager();

	/*
	*	AddTexture - Creates a texture from an SDL_Surface and adds it to the texture map
	*	@param SDL_Renderer for creating the texture
	*	@param std::string assetId - The key for the texture in the map
	*	@param std::string filepath - Where the image is located to create the texture
	*/
	void AddTexture(Renderer& renderer, const std::string& assetId, const std::string& filePath);
	const Texture& GetTexture(const std::string& assetId);
	bool HasTexture(const std::string& assetId);
	void RemoveTexture(const std::string& assetId);

};

typedef std::unique_ptr<AssetManager> AssetManager_Ptr;