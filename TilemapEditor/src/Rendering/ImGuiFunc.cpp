#include "ImGuiFunc.h"
#include <imgui/imgui.h>
#include <SDL.h>
#include "../Utilities/FileDialogWin.h"
#include "../Utilities/FileLoader.h"
#include "../Logger/Logger.h"
#include "../MouseControl.h"
#include "../Utilities/CommandManager.h"
#include "IconsFontAwesome.h"
#include <filesystem>

namespace fs = std::filesystem;

void ImGuiFuncs::TileSetWindow(const AssetManager_Ptr& assetManager, Renderer& renderer, const glm::vec2& mouseRect)
{
	// 
	if (ImGui::Begin("Texture", &mImageLoaded))
	{
		int imageWidth = mImageWidth * 2;
		int imageHeight = mImageHeight * 2;

		ImGui::Image(assetManager->GetTexture(mAssetID).get(), ImVec2(imageWidth, imageHeight));

		int mousePosX = static_cast<int>(ImGui::GetMousePos().x - ImGui::GetWindowPos().x);
		int mousePosY = static_cast<int>(ImGui::GetMousePos().y - ImGui::GetWindowPos().y - TITLE_BAR_SIZE);

		int rows = imageHeight / (mouseRect.y * 2);
		int cols = imageWidth / (mouseRect.x * 2);

		for (int i = 0; i < cols; i++)
		{
			for (int j = 0; j < rows; j++)
			{
				auto drawList = ImGui::GetWindowDrawList();

				// Check to see if we are in the area of the desired 2D tile
				if ((mousePosX >= (imageWidth / cols) * i && mousePosX <= (imageWidth / cols) + ((imageWidth / cols) * i))
					&& (mousePosY >= (imageHeight / rows) * j && mousePosY <= (imageHeight / rows) + ((imageHeight / rows) * j)))
				{
					if (ImGui::IsItemHovered())
					{

						if (ImGui::IsMouseClicked(0))
						{
							mSrcRectX = i * mouseRect.x;
							mSrcRectY = j * mouseRect.y;
						}
					}
				}
			}
		}
		ImGui::End();
	}
}

void ImGuiFuncs::ClearLoadedFiles()
{
	// Reset All data for the current project
	mAssetID = "";
	mFileName = "";
	mImageName = "";
	mImageLoaded = false;

	mLoadedTilesets.clear();
	mTilesetLocations.clear();
	mCleared = true;
}

void ImGuiFuncs::SetWindowName(const std::string& filename)
{
	fs::path path(filename);
	std::string title = "Tilemap Editor - " + path.stem().string();
	mWindowName = title;
}

void ImGuiFuncs::OpenCheckWindow()
{
	if (!mNewFile)
		return;

	if (ImGui::Begin("Create New Canvas"))
	{
		ImGui::Text("Are you sure?");
		ImGui::Spacing();

		if (ImGui::Button("yes"))
			mCheck = true;

		ImGui::SameLine();

		if (ImGui::Button("cancel"))
			mNewFile = false;

		ImGui::End();
	}

	if (mCheck)
	{
		// Clear all of the data
		ClearLoadedFiles();
		mCheck = false;
		mNewFile = false;
		SetWindowName("");
		LOG_INFO("CLEARED ALL FILES");
	}
}

void ImGuiFuncs::UpdateShortCuts(sol::state& lua, const AssetManager_Ptr& assetManager,
	Renderer& renderer, int& canvasWidth, int& canvasHeight, int& tileSize, const std::unique_ptr<CommandManager>& commandManager)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);

	// SaveFile Dialog || Save if file is loaded
	if ((state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) && state[SDL_SCANCODE_S])
	{
		Save(assetManager, renderer, canvasWidth, canvasHeight, tileSize);
	}

	// OpenFile Dialog
	if ((state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) && state[SDL_SCANCODE_O])
	{
		OpenProject(lua, assetManager, renderer, canvasWidth, canvasHeight, tileSize);
	}

	// Call to create new canvas
	if ((state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) && state[SDL_SCANCODE_N])
	{
		mNewFile = true;
	}

	// Call to create undo last Command
	if ((state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) && state[SDL_SCANCODE_Z] 
		&& (!state[SDL_SCANCODE_RSHIFT] && !state[SDL_SCANCODE_LSHIFT]) &&!mUndone)
	{
		commandManager->Undo();
		mUndone = true;
	}
	else if (!state[SDL_SCANCODE_Z] && mUndone)
	{
		mUndone = false;
	}

	// Call to create Redo last Undo Command
	if ((state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]) && (state[SDL_SCANCODE_Z] 
		&& (state[SDL_SCANCODE_RSHIFT] || state[SDL_SCANCODE_LSHIFT])) && !mRedone)
	{
		commandManager->Redo();
		mRedone = true;
	}
	else if (!state[SDL_SCANCODE_Z] && mRedone)
	{
		mRedone = false;
	}
}

void ImGuiFuncs::OpenProject(sol::state& lua, const AssetManager_Ptr& assetManager, Renderer& renderer, int& canvasWidth, int& canvasHeight, int& tileSize)
{
	mFileDialog;
	mFileLoader;
	mFileName = mFileDialog->OpenFile();

	if (mFileName == "")
		return;

	mFileLoader->LoadProject(lua, mFileName, assetManager, renderer, mLoadedTilesets, mTilesetLocations, canvasWidth, canvasHeight, tileSize);
	SetWindowName(mFileName);
}

void ImGuiFuncs::Save(const AssetManager_Ptr& assetManager, Renderer& renderer, const int& canvasWidth, const int& canvasHeight, const int& tileSize)
{
	if (mFileName == "")
	{
		mFileName = mFileDialog->SaveFile();

		if (mFileName == "")
			return;

		mFileLoader->SaveProject(mFileName, mLoadedTilesets, mTilesetLocations, canvasWidth, canvasHeight, tileSize);
		SetWindowName(mFileName);
	}
	else
	{
		mFileLoader->SaveProject(mFileName, mLoadedTilesets, mTilesetLocations, canvasWidth, canvasHeight, tileSize);
		LOG_INFO("FILES SAVED!!");
	}
}

ImGuiFuncs::ImGuiFuncs(std::shared_ptr<MouseControl>& mouseControl)
	: mFileName("")
	, mImageName("")
	, mAssetID("")
	, mWindowName("Tilemap Editor")
	, mScaleX(4)
	, mScaleY(4)
	, mWidth(16)
	, mHeight(16)
	, mLayer(0)
	, mSrcRectX(0)
	, mSrcRectY(0)
	, mImageWidth(0)
	, mImageHeight(0)
	, mMouseRectX(16)
	, mMouseRectY(16)
	, mBoxWidth(0)
	, mBoxHeight(0)
	, mBoxOffsetX(0)
	, mBoxOffsetY(0)
	, mImageLoaded(false)
	, mExit(false)
	, mCollider(false)
	, mCleared(false)
	, mCheck(false)
	, mNewFile(false)
	, mUndone(false)
	, mRedone(false)
	, mLoadedTilesets()
	, mTilesetLocations()
	, mMouseControl(mouseControl)
{
	mFileDialog = std::make_unique<FileDialogWin>();
	mFileLoader = std::make_unique<FileLoader>();
}
ImGuiFuncs::~ImGuiFuncs()
{

}


void ImGuiFuncs::SetupImgui()
{
	// Setup ImGui Context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup ImGui Keys
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;

	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
}

void ImGuiFuncs::SetupImguiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.TabRounding = 0.0f;
	style.FrameBorderSize = 1.0f;
	style.ScrollbarRounding = 0.0f;
	style.ScrollbarSize = 10.0f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.50f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.27f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.22f, 0.50f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.27f, 0.75f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.30f, 0.30f, 0.33f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.04f, 0.04f, 0.04f, 0.75f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.26f, 0.75f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 0.75f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.62f, 0.62f, 0.62f, 0.75f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.94f, 0.92f, 0.94f, 0.75f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.41f, 0.41f, 0.41f, 0.75f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.62f, 0.62f, 0.62f, 0.75f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.62f, 0.62f, 0.62f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.33f, 0.75f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.75f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.62f, 0.62f, 0.62f, 0.75f);
	colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.22f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.37f, 0.37f, 0.39f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.30f, 0.30f, 0.33f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.12f, 0.12f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.50f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
}

void ImGuiFuncs::ShowFileMenu(sol::state& lua, const AssetManager_Ptr& assetManager,
	Renderer& renderer, int& canvasWidth, int& canvasHeight, int& tileSize)
{
	if (ImGui::MenuItem(ICON_FA_FILE " New", "Ctrl + N"))
		mNewFile = true;

	if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open", "Ctrl + O"))
		OpenProject(lua, assetManager, renderer, canvasWidth, canvasHeight, tileSize);

	if (ImGui::MenuItem(ICON_FA_SAVE " Save", "Ctrl + S"))
		Save(assetManager, renderer, canvasWidth, canvasHeight, tileSize);

	if (ImGui::MenuItem(ICON_FA_SAVE " Save As..", "Ctrl + Shift + S"))
	{
		if (mFileName == "")
		{
			mFileName = mFileDialog->SaveFile();

			if (mFileName == "")
				return;

			mFileLoader->SaveProject(mFileName, mLoadedTilesets, mTilesetLocations, canvasWidth, canvasHeight, tileSize);
		}
		else
		{
			// Create a temp string from the save dialog
			std::string filename = mFileDialog->SaveFile();

			// If the string is empty, leave the function
			if (filename == "")
				return;

			mFileLoader->SaveProject(filename, mLoadedTilesets, mTilesetLocations, canvasWidth, canvasHeight, tileSize);
			// Change the main filename to the new filename
			mFileName = filename;
		}
	}

	if (ImGui::MenuItem(ICON_FA_SAVE " Save To Lua Table"))
	{
		if (mFileName == "")
		{
			mFileName = mFileDialog->SaveFile();

			if (mFileName == "")
				return;

			mFileLoader->SaveToLuaTable(mFileName, mLoadedTilesets, mTilesetLocations, tileSize);
		}
		//else
		//{
		//	// Create a temp string from the save dialog
		//	std::string filename = mFileDialog->SaveFile();

		//	// If the string is empty, leave the function
		//	if (filename == "")
		//		return;

		//	mFileLoader->SaveProject(filename, mLoadedTilesets, mTilesetLocations, canvasWidth, canvasHeight, tileSize);
		//	// Change the main filename to the new filename
		//	mFileName = filename;
		//}
	}
	// Exit the application
	if (ImGui::MenuItem("Exit"))
		mExit = true;
}

void ImGuiFuncs::ShowToolsMenu(Renderer& renderer, const AssetManager_Ptr& assetManager)
{
	if (ImGui::MenuItem(ICON_FA_PLUS " Add Tileset"))
	{
		FileDialogWin fileDialog;
		mImageName = fileDialog.OpenImageFile();

		// If the action was cancelled or returned an empty string, exit the function
		if (mImageName == std::string() || mImageName == "")
			return;

		// Create a filesystem path so we can get the stem and save that as the AssetID for later usage
		fs::path path(mImageName);
		mAssetID = path.stem().string();

		// Add the new texture to the asset manager
		assetManager->AddTexture(renderer, mAssetID, mImageName);

		// We need to query the texture to get the image width/height. This is used for setting the src_Rect positions
		if (SDL_QueryTexture(assetManager->GetTexture(mAssetID).get(), NULL, NULL, &mImageWidth, &mImageHeight) != 0)
		{
			const char* errMsg = SDL_GetError();
			LOG_ERROR("__FUNC: Load Tileset: " + std::string(errMsg));
			mImageLoaded = false;
		}
		else
		{
			mImageLoaded = true;
			// Both of these will be used for saving the project file and 
			// for access of the assetID via ImGui combo-box
			mLoadedTilesets.push_back(mAssetID);
			mTilesetLocations.push_back(mImageName);
		}
	}
}

void ImGuiFuncs::ShowTileProperties(std::shared_ptr<MouseControl>& mouseControl, const AssetManager_Ptr& assetManager, bool collider)
{
	if (ImGui::Begin("Tile Properties"))
	{
		if (!collider)
		{
			static std::string currentTileset = "";
			static std::string prevTileSet = mAssetID;

			if (ImGui::BeginCombo("Change tileset", currentTileset.c_str()))
			{
				for (int i = 0; i < mLoadedTilesets.size(); i++)
				{
					bool isSelectable = (currentTileset == mLoadedTilesets[i]);
					if (ImGui::Selectable(mLoadedTilesets[i].c_str(), isSelectable))
						currentTileset = mLoadedTilesets[i];
					if (isSelectable)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (currentTileset != "")
			{
				if (prevTileSet != currentTileset)
				{
					mAssetID = currentTileset;
					SDL_QueryTexture(assetManager->GetTexture(mAssetID).get(), NULL, NULL, &mImageWidth, &mImageHeight);
					prevTileSet = currentTileset;
				}
				if (currentTileset != mAssetID)
					currentTileset = mAssetID;
			}
		}

		ImGui::Text("Transform Component");
		ImGui::SliderInt("X Scale", &mScaleX, 1, 10);
		ImGui::SliderInt("Y Scale", &mScaleY, 1, 10);
		
		// If not creating colliders
		if (!collider)
		{
			ImGui::Text("Sprite Component");
			if (ImGui::InputInt("Layer", &mLayer, 1, 10))
			{
				// Clamp the layer [0 <= mLayer <= 10]
				if (mLayer <= 0)
					mLayer = 0;
				if (mLayer >= 10)
					mLayer = 10;
			}
		}

		if (ImGui::InputInt("Mouse Rect Y", &mMouseRectY, 8, 8))
		{
			mMouseRectY = (mMouseRectY / 8) * 8;
			// Clamp mouse Rect Y at zero
			if (mMouseRectY <= 0)
				mMouseRectY = 0;
		}

		if (ImGui::InputInt("Mouse Rect X", &mMouseRectX, 8, 8))
		{
			mMouseRectX = (mMouseRectX / 8) * 8;
			// Clamp mouse Rect Y at zero
			if (mMouseRectX <= 0)
				mMouseRectX = 0;
		}

		// If we are making box colliders, mColliders is always true
		if (collider)
			mCollider = true;
		else
			ImGui::Checkbox("Box Collider", &mCollider);

		// Set the collider for creating tiles/colliders
		mouseControl->SetCollider(mCollider);

		if (mCollider)
		{
			if (ImGui::InputInt("Box Width", &mBoxWidth, 8, 8))
			{
				// Clamp Box Width at zero
				if (mBoxWidth <= 0)
					mBoxWidth = 0;
			}

			if (ImGui::InputInt("Box Height", &mBoxHeight, 8, 8))
			{
				// Clamp Box Height at zero
				if (mBoxHeight <= 0)
					mBoxHeight = 0;
			}

			ImGui::InputInt("Box Offset X", &mBoxOffsetX, 8, 8);
			ImGui::InputInt("Box Offset Y", &mBoxOffsetY, 8, 8);
		}

		if (ImGui::Button("Set Tile Properties"))
		{
			mWidth = mMouseRectX;
			mHeight = mMouseRectY;

			mouseControl->SetTransformScale(mScaleX, mScaleY);
			mouseControl->SetMouseRect(mMouseRectX, mMouseRectY);

			if (mCollider)
				mouseControl->SetBoxColliderProperties(mBoxWidth, mBoxHeight, mBoxOffsetX, mBoxOffsetY);
		}
		// The box collider does not have a sprite
		if (!collider)
			mouseControl->SetSpriteProperties(mAssetID, mWidth, mHeight, mLayer, mSrcRectX, mSrcRectY);

		ImGui::End();
	}
}
