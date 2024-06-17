#include "Rendering/ImGuiFunc.h"
#include "Rendering/IconsFontAwesome.h"
#include "Utilities/FileLoader.h"
#include "Utilities/FileDialog.h"
#include "Utilities/CommandManager.h"

#include "Logger/Logger.h"
#include "MouseControl.h"

#include <imgui.h>
#include <SDL.h>
#include <filesystem>

namespace fs = std::filesystem;

void ImGuiFuncs::TileSetWindow( const AssetManager_Ptr& assetManager, const glm::vec2& mouseRect )
{
	//
	if ( ImGui::Begin(
			 "Texture", &mImageLoaded, ImGuiWindowFlags_HorizontalScrollbar ) ) // We also want the horizontal bar
	{
		// The image might be larger than the ImGui window, so we want to add the current scroll value to the
		// mouse position for the selected image
		auto scrollY = ImGui::GetScrollY();
		auto scrollX = ImGui::GetScrollX();

		int imageWidth = mImageWidth * 2;
		int imageHeight = mImageHeight * 2;

		ImGui::Image( assetManager->GetTexture( mAssetID ).get(), ImVec2( imageWidth, imageHeight ) );

		int mousePosX = static_cast<int>( ImGui::GetMousePos().x - ImGui::GetWindowPos().x + scrollX );
		int mousePosY = static_cast<int>( ImGui::GetMousePos().y - ImGui::GetWindowPos().y - TITLE_BAR_SIZE + scrollY );

		int rows = imageHeight / ( mouseRect.y * 2 );
		int cols = imageWidth / ( mouseRect.x * 2 );

		for ( int i = 0; i < cols; i++ )
		{
			for ( int j = 0; j < rows; j++ )
			{
				auto drawList = ImGui::GetWindowDrawList();

				// Check to see if we are in the area of the desired 2D tile
				if ( ( mousePosX >= ( imageWidth / cols ) * i &&
					   mousePosX <= ( imageWidth / cols ) + ( ( imageWidth / cols ) * i ) ) &&
					 ( mousePosY >= ( imageHeight / rows ) * j &&
					   mousePosY <= ( imageHeight / rows ) + ( ( imageHeight / rows ) * j ) ) )
				{
					if ( ImGui::IsItemHovered() )
					{

						if ( ImGui::IsMouseClicked( 0 ) )
						{
							mTileProps.srcRectX = i * mouseRect.x;
							mTileProps.srcRectY = j * mouseRect.y;
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

void ImGuiFuncs::SetWindowName( const std::string& filename )
{
	fs::path path( filename );
	std::string title = "JADE Tilemap Editor - " + path.stem().string();
	mWindowName = title;
}

void ImGuiFuncs::OpenCheckWindow()
{
	if ( !mNewFile )
		return;

	if ( ImGui::Begin( "Create New Canvas" ) )
	{
		ImGui::Text( "Are you sure?" );
		ImGui::Spacing();

		if ( ImGui::Button( "yes" ) )
			mCheck = true;

		ImGui::SameLine();

		if ( ImGui::Button( "cancel" ) )
			mNewFile = false;

		ImGui::End();
	}

	if ( mCheck )
	{
		// Clear all of the data
		ClearLoadedFiles();
		mCheck = false;
		mNewFile = false;
		SetWindowName( "" );
		LOG_INFO( "CLEARED ALL FILES" );
	}
}

void ImGuiFuncs::UpdateShortCuts( sol::state& lua, const AssetManager_Ptr& assetManager, Renderer& renderer,
								  std::shared_ptr<Canvas>& canvas, int& tileSize,
								  const std::unique_ptr<CommandManager>& commandManager )
{
	const Uint8* state = SDL_GetKeyboardState( NULL );

	// SaveFile Dialog || Save if file is loaded
	if ( ( state[ SDL_SCANCODE_LCTRL ] || state[ SDL_SCANCODE_RCTRL ] ) && state[ SDL_SCANCODE_S ] )
	{
		Save( assetManager, renderer, canvas->GetWidth(), canvas->GetHeight(), tileSize );
	}

	// OpenFile Dialog
	if ( ( state[ SDL_SCANCODE_LCTRL ] || state[ SDL_SCANCODE_RCTRL ] ) && state[ SDL_SCANCODE_O ] )
	{
		OpenProject( lua, assetManager, renderer, canvas, tileSize );
	}

	// Call to create new canvas
	if ( ( state[ SDL_SCANCODE_LCTRL ] || state[ SDL_SCANCODE_RCTRL ] ) && state[ SDL_SCANCODE_N ] )
	{
		mNewFile = true;
	}

	// Call to create undo last Command
	if ( ( state[ SDL_SCANCODE_LCTRL ] || state[ SDL_SCANCODE_RCTRL ] ) && state[ SDL_SCANCODE_Z ] &&
		 ( !state[ SDL_SCANCODE_RSHIFT ] && !state[ SDL_SCANCODE_LSHIFT ] ) && !mUndone )
	{
		commandManager->Undo();
		mUndone = true;
	}
	else if ( !state[ SDL_SCANCODE_Z ] && mUndone )
	{
		mUndone = false;
	}

	// Call to create Redo last Undo Command
	if ( ( state[ SDL_SCANCODE_LCTRL ] || state[ SDL_SCANCODE_RCTRL ] ) &&
		 ( state[ SDL_SCANCODE_Z ] && ( state[ SDL_SCANCODE_RSHIFT ] || state[ SDL_SCANCODE_LSHIFT ] ) ) && !mRedone )
	{
		commandManager->Redo();
		mRedone = true;
	}
	else if ( !state[ SDL_SCANCODE_Z ] && mRedone )
	{
		mRedone = false;
	}
}

void ImGuiFuncs::OpenProject( sol::state& lua, const AssetManager_Ptr& assetManager, Renderer& renderer,
							  std::shared_ptr<Canvas>& canvas, int& tileSize )
{
	mFileName = mFileDialog->OpenFile();

	if ( mFileName == "" )
		return;

	mFileLoader->LoadProject(
		lua, mFileName, assetManager, renderer, mLoadedTilesets, mTilesetLocations, canvas, tileSize );
	SetWindowName( mFileName );
}

void ImGuiFuncs::Save( const AssetManager_Ptr& assetManager, Renderer& renderer, const int& canvasWidth,
					   const int& canvasHeight, const int& tileSize )
{
	if ( mFileName == "" )
	{
		mFileName = mFileDialog->SaveFile();

		if ( mFileName == "" )
			return;

		mFileLoader->SaveProject( mFileName, mLoadedTilesets, mTilesetLocations, canvasWidth, canvasHeight, tileSize );
		SetWindowName( mFileName );
	}
	else
	{
		mFileLoader->SaveProject( mFileName, mLoadedTilesets, mTilesetLocations, canvasWidth, canvasHeight, tileSize );
		LOG_INFO( "FILES SAVED!!" );
	}
}

ImGuiFuncs::ImGuiFuncs( std::shared_ptr<MouseControl>& mouseControl )
	: mFileName( "" )
	, mImageName( "" )
	, mAssetID( "" )
	, mWindowName( "JADE Tilemap Editor" )
	, mLuaTableFile( "" )
	, mTileProps()
	, mPrevTileProps()
	, mWidth( 16 )
	, mHeight( 16 )
	, mImageWidth( 0 )
	, mImageHeight( 0 )
	, mImageLoaded( false )
	, mExit( false )
	, mCollider( false )
	, mAnimated( false )
	, mCleared( false )
	, mCheck( false )
	, mNewFile( false )
	, mUndone( false )
	, mRedone( false )
	, mLoadedTilesets()
	, mTilesetLocations()
	, mMouseControl( mouseControl )
{
	mFileDialog = std::make_unique<FileDialog>();
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

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	// Setup ImGui Keys
	io.KeyMap[ ImGuiKey_Backspace ] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ ImGuiKey_Enter ] = SDL_SCANCODE_RETURN;
	io.KeyMap[ ImGuiKey_Space ] = SDL_SCANCODE_SPACE;
	io.KeyMap[ ImGuiKey_Tab ] = SDL_SCANCODE_TAB;
	io.KeyMap[ ImGuiKey_LeftArrow ] = SDL_SCANCODE_LEFT;
	io.KeyMap[ ImGuiKey_RightArrow ] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ ImGuiKey_UpArrow ] = SDL_SCANCODE_UP;
	io.KeyMap[ ImGuiKey_DownArrow ] = SDL_SCANCODE_DOWN;
	io.KeyMap[ ImGuiKey_Home ] = SDL_SCANCODE_HOME;
	io.KeyMap[ ImGuiKey_End ] = SDL_SCANCODE_END;
	io.KeyMap[ ImGuiKey_Escape ] = SDL_SCANCODE_ESCAPE;

	io.KeyMap[ ImGuiKey_A ] = SDL_SCANCODE_A;
	io.KeyMap[ ImGuiKey_C ] = SDL_SCANCODE_C;
	io.KeyMap[ ImGuiKey_V ] = SDL_SCANCODE_V;
	io.KeyMap[ ImGuiKey_X ] = SDL_SCANCODE_X;
	io.KeyMap[ ImGuiKey_Y ] = SDL_SCANCODE_Y;
	io.KeyMap[ ImGuiKey_Z ] = SDL_SCANCODE_Z;
}

void ImGuiFuncs::SetupImguiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.TabRounding = 0.0f;
	style.FrameBorderSize = 1.0f;
	style.ScrollbarRounding = 0.0f;
	style.ScrollbarSize = 10.0f;

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ ImGuiCol_Text ] = ImVec4( 0.95f, 0.95f, 0.95f, 1.00f );
	colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
	colors[ ImGuiCol_WindowBg ] = ImVec4( 0.12f, 0.12f, 0.12f, 1.00f );
	colors[ ImGuiCol_ChildBg ] = ImVec4( 0.04f, 0.04f, 0.04f, 0.50f );
	colors[ ImGuiCol_PopupBg ] = ImVec4( 0.12f, 0.12f, 0.12f, 0.94f );
	colors[ ImGuiCol_Border ] = ImVec4( 0.25f, 0.25f, 0.27f, 0.50f );
	colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	colors[ ImGuiCol_FrameBg ] = ImVec4( 0.20f, 0.20f, 0.22f, 0.50f );
	colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.25f, 0.25f, 0.27f, 0.75f );
	colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.30f, 0.30f, 0.33f, 1.00f );
	colors[ ImGuiCol_TitleBg ] = ImVec4( 0.04f, 0.04f, 0.04f, 1.00f );
	colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.04f, 0.04f, 0.04f, 1.00f );
	colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.04f, 0.04f, 0.04f, 0.75f );
	colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.18f, 0.18f, 0.19f, 1.00f );
	colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.24f, 0.24f, 0.26f, 0.75f );
	colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.41f, 0.41f, 0.41f, 0.75f );
	colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.62f, 0.62f, 0.62f, 0.75f );
	colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.94f, 0.92f, 0.94f, 0.75f );
	colors[ ImGuiCol_CheckMark ] = ImVec4( 0.60f, 0.60f, 0.60f, 1.00f );
	colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.41f, 0.41f, 0.41f, 0.75f );
	colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.62f, 0.62f, 0.62f, 0.75f );
	colors[ ImGuiCol_Button ] = ImVec4( 0.20f, 0.20f, 0.22f, 1.00f );
	colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.25f, 0.25f, 0.27f, 1.00f );
	colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
	colors[ ImGuiCol_Header ] = ImVec4( 0.18f, 0.18f, 0.19f, 1.00f );
	colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.25f, 0.25f, 0.27f, 1.00f );
	colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
	colors[ ImGuiCol_Separator ] = ImVec4( 0.25f, 0.25f, 0.27f, 1.00f );
	colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
	colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.62f, 0.62f, 0.62f, 1.00f );
	colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0.30f, 0.30f, 0.33f, 0.75f );
	colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.41f, 0.41f, 0.41f, 0.75f );
	colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.62f, 0.62f, 0.62f, 0.75f );
	colors[ ImGuiCol_Tab ] = ImVec4( 0.21f, 0.21f, 0.22f, 1.00f );
	colors[ ImGuiCol_TabHovered ] = ImVec4( 0.37f, 0.37f, 0.39f, 1.00f );
	colors[ ImGuiCol_TabActive ] = ImVec4( 0.30f, 0.30f, 0.33f, 1.00f );
	colors[ ImGuiCol_TabUnfocused ] = ImVec4( 0.12f, 0.12f, 0.12f, 0.97f );
	colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.18f, 0.18f, 0.19f, 1.00f );
	colors[ ImGuiCol_PlotLines ] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
	colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
	colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
	colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
	colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.26f, 0.59f, 0.98f, 0.50f );
	colors[ ImGuiCol_DragDropTarget ] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
	colors[ ImGuiCol_NavHighlight ] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
	colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
	colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
	colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );
	style.WindowMenuButtonPosition = ImGuiDir_Right;
}

void ImGuiFuncs::ShowFileMenu( sol::state& lua, const AssetManager_Ptr& assetManager, Renderer& renderer,
							   std::shared_ptr<Canvas>& canvas, int& tileSize )
{
	if ( ImGui::MenuItem( ICON_FA_FILE " New", "Ctrl + N" ) )
		mNewFile = true;

	if ( ImGui::MenuItem( ICON_FA_FOLDER_OPEN " Open", "Ctrl + O" ) )
		OpenProject( lua, assetManager, renderer, canvas, tileSize );

	if ( ImGui::MenuItem( ICON_FA_SAVE " Save", "Ctrl + S" ) )
		Save( assetManager, renderer, canvas->GetWidth(), canvas->GetHeight(), tileSize );

	if ( ImGui::MenuItem( ICON_FA_SAVE " Save As..", "Ctrl + Shift + S" ) )
	{
		// Get the file path from the file dialog
		std::string filename = mFileDialog->SaveFile();

		// Check to see if we got the filepath from the dialog
		// The user may have cancelled the save
		if ( filename == "" )
			return;

		// Change the filename to the dialog path
		mFileName = filename;
		// Call the save project function
		mFileLoader->SaveProject(
			mFileName, mLoadedTilesets, mTilesetLocations, canvas->GetWidth(), canvas->GetHeight(), tileSize );
	}

	if ( ImGui::MenuItem( ICON_FA_SAVE " Save To Lua Table" ) )
	{
		// Create a temp string from the save dialog
		std::string filename = mFileDialog->SaveFile();

		// If the string is empty, leave the function
		if ( filename == "" )
			return;

		// Change the main filename to the new filename
		mLuaTableFile = filename;
		// Actually save the lua table to a file
		mFileLoader->SaveToLuaTable( mLuaTableFile, mLoadedTilesets, mTilesetLocations, tileSize );
	}

	// Exit the application
	if ( ImGui::MenuItem( "Exit" ) )
		mExit = true;
}

void ImGuiFuncs::ShowToolsMenu( Renderer& renderer, const AssetManager_Ptr& assetManager )
{
	if ( ImGui::MenuItem( ICON_FA_PLUS " Add Tileset" ) )
	{
		FileDialog fileDialog;
		mImageName = fileDialog.OpenImageFile();

		// If the action was cancelled or returned an empty string, exit the function
		if ( mImageName == std::string() || mImageName == "" )
			return;

		// Create a filesystem path so we can get the stem and save that as the AssetID for later usage
		fs::path path( mImageName );
		mAssetID = path.stem().string();

		// Check to see if tileset is already being used
		for ( const auto& assetIDs : mLoadedTilesets )
			if ( assetIDs == mAssetID )
				return;

		// Add the new texture to the asset manager
		assetManager->AddTexture( renderer, mAssetID, mImageName );

		// We need to query the texture to get the image width/height. This is used for setting the src_Rect positions
		if ( SDL_QueryTexture( assetManager->GetTexture( mAssetID ).get(), NULL, NULL, &mImageWidth, &mImageHeight ) !=
			 0 )
		{
			const char* errMsg = SDL_GetError();
			LOG_ERROR( "__FUNC: Load Tileset: " + std::string( errMsg ) );
			mImageLoaded = false;
		}
		else
		{
			mImageLoaded = true;
			// Both of these will be used for saving the project file and
			// for access of the assetID via ImGui combo-box
			mLoadedTilesets.push_back( mAssetID );
			mTilesetLocations.push_back( mImageName );
		}
	}
}

bool ImGuiFuncs::CheckCollidersStatus()
{
	bool statusChanged = false;
	if ( mTileProps.boxWidth != mPrevTileProps.boxWidth )
	{
		mPrevTileProps.boxWidth = mTileProps.boxWidth;
		statusChanged = true;
	}

	if ( mTileProps.boxHeight != mPrevTileProps.boxHeight )
	{
		mPrevTileProps.boxHeight = mTileProps.boxHeight;
		statusChanged = true;
	}

	if ( mTileProps.boxOffsetX != mPrevTileProps.boxOffsetX )
	{
		mPrevTileProps.boxOffsetX = mTileProps.boxOffsetX;
		statusChanged = true;
	}

	if ( mTileProps.boxOffsetY != mPrevTileProps.boxOffsetY )
	{
		mPrevTileProps.boxOffsetY = mTileProps.boxOffsetY;
		statusChanged = true;
	}

	return statusChanged;
}

bool ImGuiFuncs::CheckTransformStatus()
{
	bool statusChanged = false;

	if ( mTileProps.scaleX != mPrevTileProps.scaleX )
	{
		mPrevTileProps.scaleX = mTileProps.scaleX;
		statusChanged = true;
	}

	if ( mTileProps.scaleY != mPrevTileProps.scaleY )
	{
		mPrevTileProps.scaleY = mTileProps.scaleY;
		statusChanged = true;
	}

	if ( mTileProps.mouseRectX != mPrevTileProps.mouseRectX )
	{
		mPrevTileProps.mouseRectX = mTileProps.mouseRectX;
		mWidth = mTileProps.mouseRectX;
		statusChanged = true;
	}

	if ( mTileProps.mouseRectY != mPrevTileProps.mouseRectY )
	{
		mPrevTileProps.mouseRectY = mTileProps.mouseRectY;
		mHeight = mTileProps.mouseRectY;
		statusChanged = true;
	}

	return statusChanged;
}

bool ImGuiFuncs::CheckAnimationStatus()
{
	bool statusChanged = false;

	if ( mTileProps.numFrames != mPrevTileProps.numFrames )
	{
		mPrevTileProps.numFrames = mTileProps.numFrames;
		statusChanged = true;
	}

	if ( mTileProps.frameSpeed != mPrevTileProps.frameSpeed )
	{
		mPrevTileProps.frameSpeed = mTileProps.frameSpeed;
		statusChanged = true;
	}

	if ( mTileProps.vertical != mPrevTileProps.vertical )
	{
		mPrevTileProps.vertical = mTileProps.vertical;
		statusChanged = true;
	}

	if ( mTileProps.looped != mPrevTileProps.looped )
	{
		mPrevTileProps.looped = mTileProps.looped;
		statusChanged = true;
	}

	return statusChanged;
}
void ImGuiFuncs::ShowTileProperties( std::shared_ptr<MouseControl>& mouseControl, const AssetManager_Ptr& assetManager,
									 bool collider )
{
	std::string boxName = "Tile Properties";

	if ( collider )
		boxName = "Box Collider Properties";

	if ( ImGui::Begin( boxName.c_str() ) )
	{
		if ( !collider )
		{
			static std::string currentTileset = "";
			static std::string prevTileSet = mAssetID;

			if ( ImGui::BeginCombo( "Change tileset", currentTileset.c_str() ) )
			{
				for ( int i = 0; i < mLoadedTilesets.size(); i++ )
				{
					bool isSelectable = ( currentTileset == mLoadedTilesets[ i ] );
					if ( ImGui::Selectable( mLoadedTilesets[ i ].c_str(), isSelectable ) )
						currentTileset = mLoadedTilesets[ i ];
					if ( isSelectable )
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if ( currentTileset != "" )
			{
				if ( prevTileSet != currentTileset )
				{
					mAssetID = currentTileset;
					SDL_QueryTexture(
						assetManager->GetTexture( mAssetID ).get(), NULL, NULL, &mImageWidth, &mImageHeight );
					prevTileSet = currentTileset;
				}
				if ( currentTileset != mAssetID )
					currentTileset = mAssetID;
			}
		}

		ImGui::Text( "Transform Component" );
		ImGui::SliderInt( "X Scale", &mTileProps.scaleX, 1, 10 );
		ImGui::SliderInt( "Y Scale", &mTileProps.scaleY, 1, 10 );

		// If not creating colliders
		if ( !collider )
		{
			ImGui::Text( "Sprite Component" );
			if ( ImGui::InputInt( "Layer", &mTileProps.layer, 1, 10 ) )
			{
				// Clamp the layer [0 <= mLayer <= 10]
				if ( mTileProps.layer <= 0 )
					mTileProps.layer = 0;
				if ( mTileProps.layer >= 10 )
					mTileProps.layer = 10;
			}
		}

		if ( ImGui::InputInt( "Mouse Rect X", &mTileProps.mouseRectX, 8, 8 ) )
		{
			mTileProps.mouseRectX = ( mTileProps.mouseRectX / 8 ) * 8;
			// Clamp mouse Rect Y at zero
			if ( mTileProps.mouseRectX <= 0 )
				mTileProps.mouseRectX = 0;
		}

		if ( ImGui::InputInt( "Mouse Rect Y", &mTileProps.mouseRectY, 8, 8 ) )
		{
			mTileProps.mouseRectY = ( mTileProps.mouseRectY / 8 ) * 8;
			// Clamp mouse Rect Y at zero
			if ( mTileProps.mouseRectY <= 0 )
				mTileProps.mouseRectY = 0;
		}

		// If we are making box colliders, mColliders is always true
		if ( collider )
			mCollider = true;
		else
			ImGui::Checkbox( "Box Collider", &mCollider );

		// Set the collider for creating tiles/colliders
		mouseControl->SetCollider( mCollider );

		if ( mCollider )
		{
			if ( ImGui::InputInt( "Box Width", &mTileProps.boxWidth, 8, 8 ) )
			{
				// Clamp Box Width at zero
				if ( mTileProps.boxWidth <= 0 )
					mTileProps.boxWidth = 0;
			}

			if ( ImGui::InputInt( "Box Height", &mTileProps.boxHeight, 8, 8 ) )
			{
				// Clamp Box Height at zero
				if ( mTileProps.boxHeight <= 0 )
					mTileProps.boxHeight = 0;
			}

			ImGui::InputInt( "Box Offset X", &mTileProps.boxOffsetX, 8, 8 );
			ImGui::InputInt( "Box Offset Y", &mTileProps.boxOffsetY, 8, 8 );
		}

		if ( ImGui::Checkbox( "Animation", &mAnimated ) )
		{
			mouseControl->SetAnimated( mAnimated );
		}

		if ( mAnimated )
		{
			if ( ImGui::InputInt( "Num Frames", &mTileProps.numFrames, 1, 1 ) )
			{
				if ( mTileProps.numFrames <= 0 )
					mTileProps.numFrames = 0;
			}

			if ( ImGui::InputInt( "Frame Speed", &mTileProps.frameSpeed, 1, 1 ) )
			{
				if ( mTileProps.frameSpeed <= 0 )
					mTileProps.frameSpeed = 0;
			}

			if ( ImGui::InputInt( "Frame Offset", &mTileProps.frameOffset, 1, 1 ) )
			{
				if ( mTileProps.frameOffset <= 0 )
					mTileProps.frameOffset = 0;
			}

			ImGui::Checkbox( "Vertical", &mTileProps.vertical );
			ImGui::Checkbox( "Looped", &mTileProps.looped );
		}

		// The box collider does not have a sprite
		if ( !collider )
			mouseControl->SetSpriteProperties(
				mAssetID, mWidth, mHeight, mTileProps.layer, mTileProps.srcRectX, mTileProps.srcRectY );

		if ( CheckTransformStatus() )
		{
			mouseControl->SetTransformScale( mTileProps.scaleX, mTileProps.scaleY );
			mouseControl->SetMouseRect( mTileProps.mouseRectX, mTileProps.mouseRectY );
		}

		if ( mCollider && CheckCollidersStatus() )
			mouseControl->SetBoxColliderProperties(
				mTileProps.boxWidth, mTileProps.boxHeight, mTileProps.boxOffsetX, mTileProps.boxOffsetY );

		if ( mAnimated && CheckAnimationStatus() )
			mouseControl->SetAnimationProperties( mTileProps.numFrames,
												  mTileProps.frameSpeed,
												  mTileProps.vertical,
												  mTileProps.looped,
												  mTileProps.srcRectX );

		ImGui::End();
	}
}
