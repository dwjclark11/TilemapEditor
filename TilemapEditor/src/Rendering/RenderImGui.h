#pragma once
#include "../ECS/ECS.h"
#include "../Utilities/Utilities.h"

#include <memory>

class RenderGuiSystem : public System
{
private:
	std::unique_ptr<class ImGuiFuncs> mImFuncs;
	std::unique_ptr<class MouseControlSystem> mMouseControl;

	bool mCreateTiles;
	int mCanvasWidth;
	int mCanvasHeight;
	int mTileSize;

public:
	RenderGuiSystem();
	~RenderGuiSystem();

	void Update(const std::unique_ptr<class AssetManager>& assetManager, std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& mouseBox, SDL_Rect& camera, SDL_Event& event);
	void RenderGrid(std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& camera);
};