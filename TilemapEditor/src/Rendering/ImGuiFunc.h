#pragma once
#include <memory>
#include <string>
#include "../Utilities/Utilities.h"
#include "../AssetManager.h"
#include <glm/glm.hpp>
#include <vector>
#include <sol/sol.hpp>
#include "../Canvas.h"

/*
*  This is a collection of functions that are for use with the ImGui buttons/windows/combobox etc
*/

class ImGuiFuncs
{
private:
	std::string mFileName, mImageName, mAssetID, mWindowName, mLuaTableFile;
	int mScaleX, mScaleY, mWidth, mHeight, mLayer, mSrcRectX, mSrcRectY;
	int mImageWidth, mImageHeight, mMouseRectY, mMouseRectX;
	int mBoxWidth, mBoxHeight, mBoxOffsetX, mBoxOffsetY;

	bool mImageLoaded, mExit, mCollider, mCleared, mCheck, mNewFile, mUndone, mRedone;

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

	/*
	*	All of the window/buttons/etc styles are set here.
	*/
	void SetupImguiStyle();

	/*
	*	The ShowFileMenu opens up an ImGui menu that displays your load/save/exit buttons
	*/
	void ShowFileMenu(sol::state& lua, const AssetManager_Ptr& assetManager, Renderer& renderer, std::shared_ptr<Canvas>& canvas, int& tileSize);


	void ShowToolsMenu(Renderer& renderer, const AssetManager_Ptr& assetManager);

	/*
	*  	This is an ImGui window shows the tile properties if [Create Tiles] is selected. You are able to adjust
	*	the properties of the tiles here before placing them
	*/
	void ShowTileProperties(std::shared_ptr<class MouseControl>& mouseControl, const AssetManager_Ptr& assetManager, bool collider);

	/*
	*	TileSetWindow is the ImGui window that holds the tileset to parse. Once you load a tileset, all you have to
	*	do is left click the mouse on any desired tile and this function will set the src_rect for the tile to the
	* 	selected tile.
	*	@param: AssetManager -> Retrieve the Texture from the asset manager for the ImGui::Image
	*	@param: glm::vec2& mouseRect -> This is used to set the src_rect after selecting a tile
	*/
	void TileSetWindow(const AssetManager_Ptr& assetManager, const glm::vec2& mouseRect);

	inline const bool& FilesCleared() const { return mCleared; }


	inline void SetFilesCleared(bool cleared) { mCleared = cleared; }

	const bool& GetExit() const { return mExit; }
	const bool& GetNewFile() const { return mNewFile; }

	/*
	*	Opens up a pop up window when selecting "NEW". Asks the user if they are sure they want to proceed
	*/
	void OpenCheckWindow();

	/*
	*	This has all the short cut keys functions. Save/Load/Undo/Redo/New/
	*	You are able to call the above commands from this using the shortcut keys
	*/
	void UpdateShortCuts(sol::state& lua, const AssetManager_Ptr& assetManager,
		Renderer& renderer, std::shared_ptr<Canvas>& canvas, int& tileSize, const std::unique_ptr<class CommandManager>& commandManager);

	/*
	*	OpenProject - Loads a previously saved project from a lua table project file
	*	@param - sol::state& lua -> used to read the lua table. 
	*	@param - AssetManager -> Loads the required assets into the asset manager from the lua file
	*	@param - Canvas - Sets the canvas size of the project
	*	@param - int tileSize -> Set the tilesize of the loaded project
	*/
	void OpenProject(sol::state& lua, const AssetManager_Ptr& assetManager,
		Renderer& renderer, std::shared_ptr<Canvas>& canvas, int& tileSize);

	/*
	*  Save - Checks to see if the file has already been save, if not, opens a file dialog 
	*	and saves the project by creating a new project file, tiles file and colliders file.
	*/
	void Save(const AssetManager_Ptr& assetManager,
		Renderer& renderer, const int& canvasWidth, const int& canvasHeight, const int& tileSize);

	inline const std::string& GetWindowName() const { return mWindowName; }
};