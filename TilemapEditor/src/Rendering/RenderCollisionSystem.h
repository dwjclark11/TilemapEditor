#pragma once
#include "../ECS/ECS.h"
#include "../Utilities/Utilities.h"

class RenderCollisionSystem : public System
{
public:
	RenderCollisionSystem();

	/*
	*  Update() - Renders all the colliders that are currently in the project
	*/
	void Update(std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& camera, const float& zoom);
};