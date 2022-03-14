#pragma once
#include <map>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <memory>
#include "Utilities.h"

class AssetManager
{
public:


private:



	std::map<std::string, std::unique_ptr<SDL_Texture,  Util::SDLDestroyer>> mTextures;
	//std::map<std::string, std::unique_ptr<TTF_Font>> mFonts;
	//std::map<std::string, std::unique_ptr<Mix_Music>> mMusic;
	//std::map<std::string, std::unique_ptr<Mix_Chunk>> mSoundFX;


public:
	
	AssetManager();
	~AssetManager();

	// Add Textures
	void AddTexture(std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, const std::string& assetId, const std::string& filePath);
	const std::unique_ptr<SDL_Texture, Util::SDLDestroyer>& GetTexture(const std::string& assetId);
	bool HasTexture(const std::string& assetId);
	void RemoveTexture(const std::string& assetId);

};