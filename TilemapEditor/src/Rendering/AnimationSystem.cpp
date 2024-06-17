#include "Rendering/AnimationSystem.h"
#include "Components/SpriteComponent.h"
#include "Components/AnimationComponent.h"
#include "Components/TransformComponent.h"

#include <SDL.h>

AnimationSystem::AnimationSystem()
{
	RequiredComponent<AnimationComponent>();
	RequiredComponent<SpriteComponent>();
	RequiredComponent<TransformComponent>();
}

void AnimationSystem::Update()
{
	for ( const auto& entity : GetSystemEntities() )
	{
		const auto& transform = entity.GetComponent<TransformComponent>();

		auto& animation = entity.GetComponent<AnimationComponent>();
		auto& sprite = entity.GetComponent<SpriteComponent>();

		// Set the cuurent frame
		animation.mCurrentFrame =
			( ( SDL_GetTicks() - animation.mStartTime ) * animation.mFrameSpeedRate / 1000 ) % animation.mNumFrames;

		// If the animation is a vertical scroll use this
		if ( animation.mVertical )
		{
			sprite.mSrcRect.y = animation.mCurrentFrame * sprite.mHeight;
		}
		else
		{
			sprite.mSrcRect.x = ( animation.mCurrentFrame * sprite.mWidth ) + animation.mFrameOffset;
		}
	}
}
