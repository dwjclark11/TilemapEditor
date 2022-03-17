#pragma once
#include "../ECS/ECS.h"
#include "../Utilities/Utilities.h"
#include <sol/sol.hpp>
#include <memory>

class RenderGuiSystem : public System
{
private:
	std::unique_ptr<class ImGuiFuncs> mImFuncs;
	std::unique_ptr<class MouseControl> mMouseControl;

	bool mCreateTiles, mCreateColliders, mGridSnap, mExit;

	int mCanvasWidth, mCanvasHeight, mTileSize;

	sol::state mLua;

private:
	void SetExit(bool exit) { mExit = exit; }
	void ShowMouseLocationText(SDL_Rect& mouseBox, SDL_Rect& camera, std::unique_ptr<class MouseControl>& mouseControl);
	const bool MouseOffCanvas() const;
public:
	RenderGuiSystem();
	~RenderGuiSystem();

	void Update(const std::unique_ptr<class AssetManager>& assetManager, std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer,
		SDL_Rect& mouseBox, SDL_Rect& camera, SDL_Event& event, const float& zoom, const float& dt);
	void RenderGrid(std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& camera, const float& zoom);
	const bool GetExit() const { return mExit; }
};