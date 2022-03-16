#include "RenderImGui.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_stdlib.h>
#include "ImGuiFunc.h"
#include "../Utilities/Utilities.h"
#include "../MouseControl.h"
#include "../AssetManager.h"
#include <SDL.h>


RenderGuiSystem::RenderGuiSystem()
	: mCreateTiles(false)
	, mCreateColliders(false)
	, mGridSnap(false)
	, mExit(false)
	, mCanvasWidth(640)
	, mCanvasHeight(448)
	, mTileSize(64)

{
	mImFuncs = std::make_unique<ImGuiFuncs>();
	mMouseControl = std::make_unique<MouseControl>();

	mImFuncs->SetupImgui();
	mImFuncs->SetupImguiStyle();

	// Open sol/lua libraries
	mLua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
}

RenderGuiSystem::~RenderGuiSystem()
{

}

void RenderGuiSystem::Update(const std::unique_ptr<class AssetManager>& assetManager, std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& mouseBox, SDL_Rect& camera, SDL_Event& event)
{
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			mImFuncs->ShowFileMenu(mLua, assetManager, renderer, mCanvasWidth, mCanvasHeight, mTileSize);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::InputInt("Tile Size", &mTileSize, 8, 8))
			{
				if (mTileSize <= 8)
					mTileSize = 8;
			}

			if (ImGui::InputInt("Canvas Width", &mCanvasWidth, mTileSize, mTileSize))
			{
				if (mCanvasWidth <= 640)
					mCanvasWidth = 640;
			}

			if (ImGui::InputInt("Canvas Height", &mCanvasHeight, mTileSize, mTileSize))
			{
				if (mCanvasHeight <= 480)
					mCanvasHeight = 480;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			mImFuncs->ShowToolsMenu(renderer, assetManager);
			ImGui::Checkbox("Create Tiles", &mCreateTiles);
			ImGui::Checkbox("Create Colliders", &mCreateColliders);
			ImGui::Checkbox("Grid Snap", &mGridSnap);

			ImGui::EndMenu();
		}

		// Mouse Location
		ShowMouseLocationText(mouseBox, camera, mMouseControl);

		ImGui::EndMainMenuBar();
	}

	if (mCreateTiles)
	{
		mImFuncs->TileSetWindow(assetManager, renderer, mMouseControl->GetMouseRect());
		mImFuncs->ShowTileProperties(mMouseControl, assetManager);

		if (!MouseOffCanvas())
			mMouseControl->CreateTile(assetManager, renderer, mouseBox, camera, event);
	}

	if (mCreateColliders)
	{
		// TODO: Add Collider window
		mMouseControl->CreateCollider(assetManager, renderer, mouseBox, camera, event);
	}

	mMouseControl->UpdateMousePos(camera);

	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());

	// If Mouse is over ImGui Window, disable MouseControlSystem functions
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse)
		mMouseControl->SetMouseOverImGuiWindow(true);
	else
		mMouseControl->SetMouseOverImGuiWindow(false);


	// Check for Exit
	SetExit(mImFuncs->GetExit());

	// Check for GridSnap
	mMouseControl->SetGridSnap(mGridSnap);
}

void RenderGuiSystem::RenderGrid(std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& camera, const float& zoom)
{
	auto xTiles = (mCanvasWidth / mTileSize) * zoom;
	auto yTiles = (mCanvasHeight / mTileSize) * zoom;

	SDL_SetRenderDrawColor(renderer.get(), 70, 70, 70, 70);

	for (int i = 0; i < yTiles; i++)
	{
		for (int j = 0; j < xTiles; j++)
		{
			SDL_Rect newRect = { (j * mTileSize) - camera.x, (i * mTileSize) - camera.y, mTileSize * zoom, mTileSize * zoom};
			SDL_RenderDrawRect(renderer.get(), &newRect);
		}
	}
}

void RenderGuiSystem::ShowMouseLocationText(SDL_Rect& mouseBox, SDL_Rect& camera, std::unique_ptr<MouseControl>& mouseControl)
{
	if (!mMouseControl->MouseOutOfBounds() && mCreateTiles)
	{
		ImGui::TextColored(ImVec4(0, 255, 0, 1), "Grid [X: %d, Y: %d]", (mouseBox.x + camera.x) / mTileSize, (mouseBox.y + camera.y) / mTileSize);
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		if (mGridSnap)
			ImGui::TextColored(ImVec4(0, 255, 0, 1), "Mouse [X: %d, Y: %d]", (mouseBox.x + camera.x), (mouseBox.y + camera.y));
		else
			ImGui::TextColored(ImVec4(0, 255, 0, 1), "Mouse [X: %d, Y: %d]", static_cast<int>(mouseControl->GetMousePosScreen().x), static_cast<int>(mouseControl->GetMousePosScreen().y));
	}

}

const bool RenderGuiSystem::MouseOffCanvas() const
{
	if (mMouseControl->GetMousePosScreen().x > mCanvasWidth || mMouseControl->GetMousePosScreen().y > mCanvasHeight)
		return true;

	return false;
}
