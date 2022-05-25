#include "RenderCollisionSystem.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL.h>

RenderCollisionSystem::RenderCollisionSystem()
{
	RequiredComponent<BoxColliderComponent>();
	RequiredComponent<TransformComponent>();
}

void RenderCollisionSystem::Update(std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& camera, const float& zoom)
{
	for (const auto& entity : GetSystemEntities())
	{
		const auto& transform = entity.GetComponent<TransformComponent>();
		const auto& collider = entity.GetComponent<BoxColliderComponent>();

		const SDL_Rect srcRect = {
			std::floor((transform.mPosition.x + collider.mOffset.x) * zoom) - camera.x,
			std::floor((transform.mPosition.y + collider.mOffset.y) * zoom) - camera.y,
			std::ceil(collider.mWidth * transform.mScale.x * zoom ),
			std::ceil(collider.mHeight * transform.mScale.y * zoom)
		};

		SDL_SetRenderDrawColor(renderer.get(), 255, 0, 0, 125);
		SDL_RenderFillRect(renderer.get(), &srcRect);
		SDL_RenderDrawRect(renderer.get(), &srcRect);
	}
}
