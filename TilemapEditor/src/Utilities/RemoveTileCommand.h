#pragma once
#include "ICommand.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"

class RemoveTileCommand : public ICommand
{
private:
	std::shared_ptr<class MouseControl> mMouseControl;
	
	int mTileId;
	bool mCollider;

	BoxColliderComponent mBoxColliderComponent;
	TransformComponent mTransformComponent;
	SpriteComponent mSpriteComponent;

public:
	RemoveTileCommand(std::shared_ptr<class MouseControl>& mouseControl);
	virtual void Execute() override;
	virtual void Undo() override;
	virtual void Redo() override;
};