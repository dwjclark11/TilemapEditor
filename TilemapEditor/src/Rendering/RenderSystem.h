#pragma once
#include "../ECS/ECS.h"

class RenderSystem : public System
{
public:
	RenderSystem();

	void Update(struct SDL_Renderer* renderer, std::unique_ptr<class AssetManager>& assetManager, struct SDL_Rect& camera);
};