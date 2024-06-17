#pragma once

#include <SDL.h>
#include <string>
#include <glm/glm.hpp>

struct SpriteComponent
{
	std::string mAssetId;
	int mWidth;
	int mHeight;
	int mLayer;
	bool mIsFixed;
	SDL_RendererFlip mFlip;
	SDL_Rect mSrcRect;
	glm::vec2 mOffset;

	SpriteComponent( std::string assetId = "", int width = 0, int height = 0, int layer = 0, bool isfixed = 0,
					 int srcRectX = 0, int srcRectY = 0, glm::vec2 offset = glm::vec2( 0 ) )
	{
		mAssetId = assetId;
		mWidth = width;
		mHeight = height;
		mLayer = layer;
		mIsFixed = isfixed;
		mFlip = SDL_FLIP_NONE;
		mSrcRect = { srcRectX, srcRectY, width, height };
		mOffset = offset;
	}
};