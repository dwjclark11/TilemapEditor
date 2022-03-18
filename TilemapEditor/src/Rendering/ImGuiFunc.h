#pragma once
#include <memory>
#include <string>
#include "../Utilities/Utilities.h"
#include <glm/glm.hpp>
#include <vector>
#include <sol/sol.hpp>

class ImGuiFuncs
{
private:
	std::string mFileName, mImageName, mAssetID;
	int mScaleX, mScaleY, mWidth, mHeight, mLayer, mSrcRectX, mSrcRectY;
	int mImageWidth, mImageHeight, mMouseRectY, mMouseRectX;
	int mBoxWidth, mBoxHeight, mBoxOffsetX, mBoxOffsetY;

	bool mImageLoaded, mExit, mCollider, mCleared, mCheck, mCloseWindow;

	const int TITLE_BAR_SIZE = 26;

	std::vector<std::string> mLoadedTilesets;
	std::vector<std::string> mTilesetLocations;

	std::unique_ptr<class FileDialog> mFileDialog;
	std::unique_ptr<class FileLoader> mFileLoader;

private:
	void ClearLoadedFiles();

public:
	ImGuiFuncs();
	~ImGuiFuncs();

	void SetupImgui();
	void SetupImguiStyle();

	void ShowFileMenu(sol::state& lua, const std::unique_ptr<class AssetManager>& assetManager,
		std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, int& canvasWidth, int& canvasHeight, int& tileSize);
	void ShowToolsMenu(std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, const std::unique_ptr<class AssetManager>& assetManager);
	void ShowTileProperties(std::unique_ptr<class MouseControl>& mouseControl, const std::unique_ptr<class AssetManager>& assetManager, bool collider);

	void TileSetWindow(const std::unique_ptr<class AssetManager>& assetManager, std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, const glm::vec2& mouseRect);

	inline const bool& FilesCleared() const { return mCleared; }
	inline void SetFilesCleared(bool cleared) { mCleared = cleared; }
	const bool& GetExit() const { return mExit; }
	void OpenCheckWindow();

	void UpdateShortCuts(sol::state& lua, const std::unique_ptr<class AssetManager>& assetManager,
		std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, int& canvasWidth, int& canvasHeight, int& tileSize);

	void OpenProject(sol::state& lua, const std::unique_ptr<class AssetManager>& assetManager,
		std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, int& canvasWidth, int& canvasHeight, int& tileSize);

	void Save(const std::unique_ptr<class AssetManager>& assetManager,
		std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, const int& canvasWidth, const int& canvasHeight, const int& tileSize);

};