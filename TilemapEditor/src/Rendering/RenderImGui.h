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
	std::shared_ptr<class MouseControl> mMouseControl;
	std::shared_ptr<class Canvas> mCanvas;

	std::string mWindowName;

	bool mCreateTiles, mCreateColliders, mGridSnap, mExit;
	int mCanvasWidth, mPrevCanvasWidth, mCanvasHeight, mPrevCanvasHeight, mTileSize, mPrevTileSize, mGridX, mGridY;

	sol::state mLua;
	std::unique_ptr<class CommandManager> mCommandManager;

private:
	void SetExit(bool exit) { mExit = exit; }
	void ShowMouseLocationText(SDL_Rect& mouseBox, SDL_Rect& camera);
	const bool MouseOffCanvas() const;
	void UpdateCanvas();

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
	inline void SetWindowName(Window& window) { SDL_SetWindowTitle(window.get(), mWindowName.c_str()); }
	inline const bool& GetExit() const { return mExit; }
};