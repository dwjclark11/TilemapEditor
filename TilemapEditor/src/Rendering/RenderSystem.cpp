#include "RenderSystem.h"
#include <vector>
#include "../AssetManager.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"

RenderSystem::RenderSystem()
{
	RequiredComponent<TransformComponent>();
	RequiredComponent<SpriteComponent>();

}

void RenderSystem::Update(SDL_Renderer* renderer, std::unique_ptr<class AssetManager>& assetManager, SDL_Rect& camera, const float& zoom)
{
	// Create a struct for sorting entities
	struct RenderableEntity
	{
		TransformComponent transformComponent;
		SpriteComponent spriteComponent;
	};

	// Create a vector container for renderable entities
	std::vector<RenderableEntity> renderableEntities;

	for (const auto& entity : GetSystemEntities())
	{
		RenderableEntity renderableEntity;
		renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
		renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();

		bool isEntityOutsideCamera = (
			renderableEntity.transformComponent.mPosition.x + (renderableEntity.transformComponent.mScale.x * renderableEntity.spriteComponent.mWidth) < camera.x ||
			renderableEntity.transformComponent.mPosition.x > camera.x + camera.w ||
			renderableEntity.transformComponent.mPosition.y + (renderableEntity.transformComponent.mScale.y * renderableEntity.spriteComponent.mHeight) < camera.y ||
			renderableEntity.transformComponent.mPosition.y > camera.y + camera.h
			);

		// Bypass entites that are outside of the camera and fixed sprites
		//if (isEntityOutsideCamera && !renderableEntity.spriteComponent.mIsFixed)
		//{
		//	continue;
		//}

		// place the entity inside of the Renderable entities vector
		renderableEntities.emplace_back(renderableEntity);
	}

	// Sort the entities based on their layer (z-index)
	std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity a, const RenderableEntity b)
		{
			return a.spriteComponent.mLayer < b.spriteComponent.mLayer;
		});

	// Draw all of the Entities
	for (const auto& entity : renderableEntities)
	{
		const auto& transform = entity.transformComponent;
		const auto& sprite = entity.spriteComponent;

		// Set the src Rect of our original sprite Texture
		SDL_Rect srcRect = sprite.mSrcRect;

		// Set the Destination rect with the x, y position to be rendered

		SDL_Rect dstRect = {
			(std::floor(transform.mPosition.x * zoom) - (sprite.mIsFixed ? 0 : camera.x)), 
			(std::floor(transform.mPosition.y * zoom) - (sprite.mIsFixed ? 0 : camera.y)),
			std::ceil(sprite.mWidth * transform.mScale.x * zoom),
			std::ceil(sprite.mHeight * transform.mScale.y * zoom)
		};

		SDL_RenderCopyEx(
			renderer,
			assetManager->GetTexture(sprite.mAssetId).get(),
			&srcRect,
			&dstRect,
			transform.mRotation,
			NULL,
			sprite.mFlip
		);
	}
}
