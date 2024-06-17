#include "AssetManager.h"
#include "Logger/Logger.h"

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
}

void AssetManager::AddTexture( Renderer& renderer, const std::string& assetId, const std::string& filePath )
{
	if ( !HasTexture( assetId ) )
	{
		SDL_Surface* surface = IMG_Load( filePath.c_str() );
		if ( !surface )
			LOG_ERROR( "Unable to make Surface!" );

		Texture texture = Texture( SDL_CreateTextureFromSurface( renderer.get(), surface ) );

		if ( !texture )
		{
			LOG_ERROR( "Unable to Create Texture[{0}] at filepath [{1}]", assetId, filePath );
		}

		// Fre the surface once the texture is created
		SDL_FreeSurface( surface );

		// Add the Textures to the map
		mTextures.emplace( assetId, std::move( texture ) );
	}
	else
	{
		LOG_ERROR( "ASSETMANAGER.CPP__LINE__36: Texture [{0}] already exists!" );
	}
}

const Texture& AssetManager::GetTexture( const std::string& assetId )
{
	return mTextures[ assetId ];
}

bool AssetManager::HasTexture( const std::string& assetId )
{
	return mTextures.find( assetId ) != mTextures.end();
}

void AssetManager::RemoveTexture( const std::string& assetId )
{
}
