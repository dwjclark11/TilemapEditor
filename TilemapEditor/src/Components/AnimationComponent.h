#pragma once
#include <SDL.h>

class AnimationComponent
{
public:
	int mNumFrames;
	int mCurrentFrame;
	int mFrameSpeedRate;
	bool mVertical;
	bool mIsLooped;
	int mStartTime;
	int mFrameOffset;
	int mLastFrame;

	AnimationComponent(int numFrames = 1, int frameSpeedRate = 1, bool vertical = true, bool isLooped = true, int frameOffset = 0)
	{
		this->mNumFrames = numFrames;
		this->mCurrentFrame = 1;
		this->mFrameSpeedRate = frameSpeedRate;
		this->mVertical = vertical;
		this->mIsLooped = isLooped;
		this->mStartTime = SDL_GetTicks();
		this->mFrameOffset = frameOffset;
		this->mLastFrame = 0;
	}
};