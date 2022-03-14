#include "AssetManager.h"

AssetManager::AssetManager()
{

}

AssetManager::~AssetManager()
{
}

void AssetManager::AddTexture(std::unique_ptr<SDL_Renderer>& renderer, const std::string& assetId, const std::string& filePath)
{
	if (!HasTexture(assetId))
	{
		SDL_Surface* surface = IMG_Load(filePath.c_str());
		std::unique_ptr<SDL_Texture, SDL_Deleter> texture = std::unique_ptr<SDL_Texture, SDL_Deleter>(SDL_CreateTextureFromSurface(renderer.get(), std::move(surface)));

		if (!texture)
		{
			// TODO: ERROR!
		}

		// Fre the surface once the texture is created
		SDL_FreeSurface(surface);

		// Add the Textures to the map
		mTextures.emplace(assetId, std::move(texture));
	}
}

const std::unique_ptr<SDL_Texture, AssetManager::SDL_Deleter>& AssetManager::GetTexture(const std::string& assetId)
{
	return mTextures[assetId];
}

bool AssetManager::HasTexture(const std::string& assetId)
{
	return false;
}

void AssetManager::RemoveTexture(const std::string& assetId)
{

}
