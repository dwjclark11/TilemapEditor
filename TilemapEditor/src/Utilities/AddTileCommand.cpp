#include "Utilities/AddTileCommand.h"
#include "MouseControl.h"

AddTileCommand::AddTileCommand( std::shared_ptr<MouseControl>& mouseControl )
	: mMouseControl( mouseControl )
	, mTileId( -1 ) // -1 means no Id
	, mCollider( false )
	, mAnimated( false )
	, mBoxColliderComponent()
	, mTransformComponent()
	, mSpriteComponent()
	, mAnimationComponent()
{
}

void AddTileCommand::Execute()
{
	mTileId = mMouseControl->GetRecentTileId();
	LOG_INFO( "Tile ID: {0}", mTileId );
}

void AddTileCommand::Undo()
{
	auto entities = Registry::Instance().GetEntitiesByGroup( "tiles" );

	for ( auto& entity : entities )
	{
		// Remove the most Recently added tile
		if ( entity.GetID() == mTileId )
		{
			const auto& transform = entity.GetComponent<TransformComponent>();
			const auto& sprite = entity.GetComponent<SpriteComponent>();

			mTransformComponent = transform;
			mSpriteComponent = sprite;

			if ( entity.HasComponent<BoxColliderComponent>() )
			{
				mCollider = true;
				const auto& boxCollider = entity.GetComponent<BoxColliderComponent>();
				mBoxColliderComponent = boxCollider;
			}

			if ( entity.HasComponent<AnimationComponent>() )
			{
				mAnimated = true;
				const auto& animation = entity.GetComponent<AnimationComponent>();
				mAnimationComponent = animation;
			}

			entity.Kill();
			LOG_INFO( "UNDO: Remove Tile: {0}", mTileId );
		}
	}
}

void AddTileCommand::Redo()
{
	Entity newEntity = Registry::Instance().CreateEntity();
	newEntity.Group( "tiles" );
	newEntity.AddComponent<TransformComponent>( mTransformComponent );
	newEntity.AddComponent<SpriteComponent>( mSpriteComponent );

	if ( mCollider )
		newEntity.AddComponent<BoxColliderComponent>( mBoxColliderComponent );

	if ( mAnimated )
		newEntity.AddComponent<AnimationComponent>( mAnimationComponent );

	mTileId = newEntity.GetID();
}
