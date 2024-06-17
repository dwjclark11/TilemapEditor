#include "Rendering/RenderCollisionSystem.h"
#include "Rendering/RenderableEntity.h"
#include "Components/BoxColliderComponent.h"
#include "Components/TransformComponent.h"

#include <SDL.h>
#include <algorithm>

RenderCollisionSystem::RenderCollisionSystem()
{
	RequiredComponent<BoxColliderComponent>();
	RequiredComponent<TransformComponent>();
}

void RenderCollisionSystem::Update( std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& camera,
									const float& zoom )
{
	// Create a vector container for renderable entities
	std::vector<RenderableEntity> renderableEntities;

	for ( const auto& entity : GetSystemEntities() )
	{
		RenderableEntity renderableEntity;
		renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
		renderableEntity.boxComponent = entity.GetComponent<BoxColliderComponent>();

		bool isEntityOutsideCamera =
			( renderableEntity.transformComponent.mPosition.x +
					  ( renderableEntity.transformComponent.mScale.x * renderableEntity.spriteComponent.mWidth ) <
				  camera.x ||
			  renderableEntity.transformComponent.mPosition.x > camera.x + camera.w ||
			  renderableEntity.transformComponent.mPosition.y +
					  ( renderableEntity.transformComponent.mScale.y * renderableEntity.spriteComponent.mHeight ) <
				  camera.y ||
			  renderableEntity.transformComponent.mPosition.y > camera.y + camera.h );

		// place the entity inside of the Renderable entities vector
		renderableEntities.emplace_back( renderableEntity );
	}

	// Sort the entities based on their layer (z-index)
	std::sort(
		renderableEntities.begin(), renderableEntities.end(), []( const RenderableEntity a, const RenderableEntity b ) {
			return a.spriteComponent.mLayer < b.spriteComponent.mLayer;
		} );

	for ( const auto& entity : renderableEntities )
	{
		const auto& transform = entity.transformComponent;
		const auto& collider = entity.boxComponent;

		const SDL_Rect srcRect = {
			static_cast<int>( std::floor( ( transform.mPosition.x + collider.mOffset.x ) * zoom ) - camera.x ),
			static_cast<int>( std::floor( ( transform.mPosition.y + collider.mOffset.y ) * zoom ) - camera.y ),
			static_cast<int>( std::ceil( collider.mWidth * transform.mScale.x * zoom ) ),
			static_cast<int>( std::ceil( collider.mHeight * transform.mScale.y * zoom ) ) };

		SDL_SetRenderDrawColor( renderer.get(), 255, 0, 0, 125 );
		SDL_RenderFillRect( renderer.get(), &srcRect );
		SDL_RenderDrawRect( renderer.get(), &srcRect );
	}
}
