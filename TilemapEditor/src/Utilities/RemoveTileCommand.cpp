#include "Utilities/RemoveTileCommand.h"
#include "MouseControl.h"

RemoveTileCommand::RemoveTileCommand( std::shared_ptr<MouseControl>& mouseControl )
	: mMouseControl( mouseControl )
	, mTileId( -1 )
	, mCollider( false )
	, mAnimated( false )
	, mBoxColliderComponent()
	, mTransformComponent()
	, mSpriteComponent()
	, mAnimationComponent()
{
}

void RemoveTileCommand::Execute()
{
	mBoxColliderComponent = mMouseControl->GetRemovedBoxComponent();

	if ( mBoxColliderComponent.mWidth == 0 && mBoxColliderComponent.mHeight == 0 &&
		 mBoxColliderComponent.mOffset == glm::vec2( 0 ) )
		mCollider = false;
	else
		mCollider = true;

	mTransformComponent = mMouseControl->GetRemovedTransform();
	mSpriteComponent = mMouseControl->GetRemovedSpriteComponent();

	mAnimationComponent = mMouseControl->GetRemovedAnimationComponent();

	if ( mAnimationComponent.mNumFrames > 1 )
		mAnimated = true;
	else
		mAnimated = false;
}

void RemoveTileCommand::Undo()
{
	// Create a new tile based on the removed tile
	Entity newEntity = Registry::Instance().CreateEntity();
	newEntity.Group( "tiles" );
	newEntity.AddComponent<TransformComponent>( mTransformComponent );
	newEntity.AddComponent<SpriteComponent>( mSpriteComponent );
	// If there is a collider, add the collider
	if ( mCollider )
		newEntity.AddComponent<BoxColliderComponent>( mBoxColliderComponent );

	if ( mAnimated )
		newEntity.AddComponent<AnimationComponent>( mAnimationComponent );

	// The Tile id is need for the redo so we can remove the tile
	mTileId = newEntity.GetID();
}

// Redo removes the tile again
void RemoveTileCommand::Redo()
{
	// If the id is -1, it was not set, leave the function
	if ( mTileId == -1 )
		return;

	auto entities = Registry::Instance().GetEntitiesByGroup( "tiles" );

	for ( auto& entity : entities )
	{
		if ( entity.GetID() == mTileId )
		{
			entity.Kill();
			LOG_INFO( "REMOVE: __REDO__LINE__58: Tile: {0} has been removed!", mTileId );
			mTileId = -1;
		}
	}
}
