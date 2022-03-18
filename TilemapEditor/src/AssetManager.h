#pragma once
#include <map>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <memory>
#include "./Utilities/Utilities.h"

class AssetManager
{
public:


private:



	std::map<std::string, Texture> mTextures;
	//std::map<std::string, std::unique_ptr<TTF_Font>> mFonts;
	//std::map<std::string, std::unique_ptr<Mix_Music>> mMusic;
	//std::map<std::string, std::unique_ptr<Mix_Chunk>> mSoundFX;


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