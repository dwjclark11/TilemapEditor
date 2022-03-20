#pragma once
#include <memory>
#include <string>
#include "../Utilities/Utilities.h"
#include "../AssetManager.h"
#include <glm/glm.hpp>
#include <vector>
#include <sol/sol.hpp>

class ImGuiFuncs
{
private:
	std::string mFileName, mImageName, mAssetID, mWindowName;
	int mScaleX, mScaleY, mWidth, mHeight, mLayer, mSrcRectX, mSrcRectY;
	int mImageWidth, mImageHeight, mMouseRectY, mMouseRectX;
	int mBoxWidth, mBoxHeight, mBoxOffsetX, mBoxOffsetY;

	bool mImageLoaded, mExit, mCollider, mCleared, mCheck, mNewFile, mUndone;

	const int TITLE_BAR_SIZE = 26;

	std::vector<std::string> mLoadedTilesets;
	std::vector<std::string> mTilesetLocations;

	std::unique_ptr<class FileDialogWin> mFileDialog;
	std::unique_ptr<class FileLoader> mFileLoader;
	std::shared_ptr<class MouseControl> mMouseControl;

private:
	void ClearLoadedFiles();
	void SetWindowName(const std::string& filename);
public:
	ImGuiFuncs(class std::shared_ptr<MouseControl>& mouseControl);
	~ImGuiFuncs();

	void SetupImgui();
	void SetupImguiStyle();

	void ShowFileMenu(sol::state& lua, const AssetManager_Ptr& assetManager, Renderer& renderer, int& canvasWidth, int& canvasHeight, int& tileSize);
	void ShowToolsMenu(Renderer& renderer, const AssetManager_Ptr& assetManager);
	void ShowTileProperties(std::shared_ptr<class MouseControl>& mouseControl, const AssetManager_Ptr& assetManager, bool collider);
	void TileSetWindow(const AssetManager_Ptr& assetManager, Renderer& renderer, const glm::vec2& mouseRect);

	inline const bool& FilesCleared() const { return mCleared; }
	inline void SetFilesCleared(bool cleared) { mCleared = cleared; }

	const bool& GetExit() const { return mExit; }
	const bool& GetNewFile() const { return mNewFile; }

	void OpenCheckWindow();

	void UpdateShortCuts(sol::state& lua, const AssetManager_Ptr& assetManager,
		Renderer& renderer, int& canvasWidth, int& canvasHeight, int& tileSize, const std::unique_ptr<class CommandManager>& commandManager);

	void OpenProject(sol::state& lua, const AssetManager_Ptr& assetManager,
		Renderer& renderer, int& canvasWidth, int& canvasHeight, int& tileSize);

	void Save(const AssetManager_Ptr& assetManager,
		Renderer& renderer, const int& canvasWidth, const int& canvasHeight, const int& tileSize);

	inline const std::string& GetWindowName() const { return mWindowName; }
};