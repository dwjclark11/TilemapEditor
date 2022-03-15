#include "ImGuiFunc.h"
#include <imgui/imgui.h>
#include <SDL.h>
#include "../Utilities/FileDialogWin.h"

#include "../Logger/Logger.h"
#include "../MouseControlSystem.h"
#include "../AssetManager.h"
#include <filesystem>

namespace fs = std::filesystem;

void ImGuiFuncs::TileSetWindow(const std::unique_ptr<AssetManager>& assetManager, std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, const glm::vec2& mouseRect)
{
	// 
	if (ImGui::Begin("Texture", &mImageLoaded))
	{
		int imageWidth = mImageWidth * 2;
		int imageHeight = mImageHeight * 2;

		ImGui::Image(assetManager->GetTexture(mAssetID).get(), ImVec2(imageWidth, imageHeight));

		int mousePosX = ImGui::GetMousePos().x - ImGui::GetWindowPos().x;
		int mousePosY = ImGui::GetMousePos().y - ImGui::GetWindowPos().y - TITLE_BAR_SIZE;

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

ImGuiFuncs::ImGuiFuncs()
	: mFileName("")
	, mImageName("")
	, mAssetID("")
	, mScaleX(1)
	, mScaleY(1)
	, mWidth(16)
	, mHeight(16)
	, mLayer(0)
	, mSrcRectX(0)
	, mSrcRectY(0)
	, mImageWidth(0)
	, mImageHeight(0)
	, mMouseRectX(8)
	, mMouseRectY(8)
	, mImageLoaded(false)
	, mExit(false)
{

}
ImGuiFuncs::~ImGuiFuncs()
{

}


void ImGuiFuncs::SetupImgui()
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

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

void ImGuiFuncs::ShowFileMenu()
{
	if (ImGui::MenuItem("New", "Ctrl + N"))
	{

	}

	if (ImGui::MenuItem("Open", "Ctrl + O"))
	{
		FileDialog fileDialog;
		std::string fileName = fileDialog.OpenFile();
	}

	if (ImGui::MenuItem("Save", "Ctrl + S"))
	{

	}

	if (ImGui::MenuItem("Save As..", "Ctrl + Shift + S"))
	{

	}

	if (ImGui::MenuItem("Exit"))
	{
		// Exit the application
		mExit = true;
	}
}

void ImGuiFuncs::ShowToolsMenu(std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, const std::unique_ptr<AssetManager>& assetManager)
{
	if (ImGui::MenuItem("Load Tileset"))
	{
		FileDialog fileDialog;
		mImageName = fileDialog.OpenImageFile();

		if (mImageName == std::string() || mImageName == "")
			return;
		fs::path path(mImageName);
		mAssetID = path.stem().string();
		assetManager->AddTexture(renderer, mAssetID, mImageName);

		if (SDL_QueryTexture(assetManager->GetTexture(mAssetID).get(), NULL, NULL, &mImageWidth, &mImageHeight) != 0)
		{
			const char* errMsg = SDL_GetError();
			LOG_ERROR("__FUNC: Load Tileset: " + std::string(errMsg));
			mImageLoaded = false;
		}
		else
		{
			mImageLoaded = true;
			LOG_INFO("Filename: {0}", path.stem().string());
		}

		
	}
}

void ImGuiFuncs::ShowTileProperties(std::unique_ptr<MouseControlSystem>& mouseControl)
{
	if (ImGui::Begin("Tile Properties"))
	{
		ImGui::Text("Transform Component");
		ImGui::SliderInt("X Scale", &mScaleX, 1, 10);
		ImGui::SliderInt("Y Scale", &mScaleY, 1, 10);
		
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

		if (ImGui::Button("Set Tile Properties"))
		{
			mWidth = mMouseRectX;
			mHeight = mMouseRectY;
			
			mouseControl->SetTransformScale(mScaleX, mScaleY);
			mouseControl->SetMouseRect(mMouseRectX, mMouseRectY);
		}
		mouseControl->SetSpriteProperties(mAssetID, mWidth, mHeight, mLayer, mSrcRectX, mSrcRectY);
		ImGui::End();
	}
}