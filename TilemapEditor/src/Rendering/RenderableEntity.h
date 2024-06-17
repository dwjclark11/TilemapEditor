#pragma once

#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/BoxColliderComponent.h"

// Create a struct for sorting entities
struct RenderableEntity
{
	TransformComponent transformComponent;
	SpriteComponent spriteComponent;
	BoxColliderComponent boxComponent;
};
