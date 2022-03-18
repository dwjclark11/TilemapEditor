#pragma once
#include "../ECS/ECS.h"
#include "../Utilities/Utilities.h"
#include "../AssetManager.h"
#include <sol/sol.hpp>
#include <memory>

class RenderGuiSystem : public System
{
private:
	std::unique_ptr<class ImGuiFuncs> mImFuncs;
	std::unique_ptr<class MouseControl> mMouseControl;

	bool mCreateTiles, mCreateColliders, mGridSnap, mExit;

	int mCanvasWidth, mCanvasHeight, mTileSize, mGridX, mGridY;

	sol::state mLua;

private:
	void SetExit(bool exit) { mExit = exit; }
	void ShowMouseLocationText(SDL_Rect& mouseBox, SDL_Rect& camera);
	const bool MouseOffCanvas() const;

public:
	RenderGuiSystem();
	~RenderGuiSystem();

	void Update(const AssetManager_Ptr& assetManager, Renderer& renderer,
		SDL_Rect& mouseBox, SDL_Rect& camera, SDL_Event& event, const float& zoom, const float& dt);

	/*
		RenderGrid: This renders a grid on the screen based on the Canvas Width/Height and the tile size.
		[gridCols] = canvasWidth / tilesize [gridRows] = canvasHeight / tilesize
		This also works with the zoom and pan functionality.
	*/
	void RenderGrid(Renderer& renderer, SDL_Rect& camera, const float& zoom);
	void CreateNewCanvas();

	const bool GetExit() const { return mExit; }
};