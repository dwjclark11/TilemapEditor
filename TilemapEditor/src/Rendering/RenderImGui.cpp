#include "RenderImGui.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_stdlib.h>
#include "ImGuiFunc.h"
#include "../Utilities/Utilities.h"
#include "../MouseControl.h"
#include <SDL.h>


RenderGuiSystem::RenderGuiSystem()
	: mCreateTiles(false)
	, mCreateColliders(false)
	, mGridSnap(false)
	, mExit(false)
	, mCanvasWidth(640)
	, mCanvasHeight(448)
	, mTileSize(64)
	, mGridX(0)
	, mGridY(0)

{
	mImFuncs = std::make_unique<ImGuiFuncs>();
	mMouseControl = std::make_unique<MouseControl>();

	// Set-Up ImGui Properties
	mImFuncs->SetupImgui();
	mImFuncs->SetupImguiStyle();

	// Open sol/lua libraries
	mLua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
}

RenderGuiSystem::~RenderGuiSystem()
{

}

void RenderGuiSystem::Update(const AssetManager_Ptr& assetManager, Renderer& renderer,
	SDL_Rect& mouseBox, SDL_Rect& camera, SDL_Event& event, const float& zoom, const float& dt)
{
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			mImFuncs->ShowFileMenu(mLua, assetManager, renderer, mCanvasWidth, mCanvasHeight, mTileSize);

			if (mImFuncs->FilesCleared())
			{
				CreateNewCanvas();
				mImFuncs->SetFilesCleared(false);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			// Clamp the minimum Tile Size
			if (ImGui::InputInt("Tile Size", &mTileSize, 8, 8))
			{
				if (mTileSize <= 8)
					mTileSize = 8;
			}

			// Clamp the minimum Canvas width
			if (ImGui::InputInt("Canvas Width", &mCanvasWidth, mTileSize, mTileSize))
			{
				if (mCanvasWidth <= 640)
					mCanvasWidth = 640;
			}

			// Clamp the minimum Canvas height
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

			// Put a space between the Tileset loader and the checkboxes
			ImGui::Spacing(); ImGui::Spacing();

			ImGui::Checkbox("Create Tiles", &mCreateTiles);
			ImGui::Checkbox("Create Colliders", &mCreateColliders);
			ImGui::Checkbox("Grid Snap", &mGridSnap);

			ImGui::EndMenu();
		}
		// Mouse Location Text inside menu bar
		ShowMouseLocationText(mouseBox, camera);
		ImGui::EndMainMenuBar();
	}

	mImFuncs->OpenCheckWindow();

	// Creating tiles
	if (mCreateTiles)
	{
		mImFuncs->TileSetWindow(assetManager, renderer, mMouseControl->GetMouseRect());
		mImFuncs->ShowTileProperties(mMouseControl, assetManager, false);

		// If the mouse is off the canvas, do not render the mouse box and do not create a new tile
		if (!MouseOffCanvas())
			mMouseControl->CreateTile(assetManager, renderer, mouseBox, camera, event);
	}
	// Creating Box Colliders
	if (mCreateColliders)
	{
		mImFuncs->ShowTileProperties(mMouseControl, assetManager, true);
		// If the mouse is off the canvas, do not render the mouse box and do not create a new collider
		if (!MouseOffCanvas())
			mMouseControl->CreateCollider(assetManager, renderer, mouseBox, camera, event);
	}

	// Check for GridSnap
	mMouseControl->SetGridSnap(mGridSnap);

	// Update Mouse functions
	mMouseControl->UpdateZoom(zoom);
	mMouseControl->PanCamera(camera, dt);
	mMouseControl->UpdateMousePos(camera);
	mMouseControl->UpdateGridSize(mTileSize);

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
	mImFuncs->UpdateShortCuts(mLua, assetManager, renderer, mCanvasWidth, mCanvasHeight, mTileSize);
}

void RenderGuiSystem::RenderGrid(Renderer& renderer, SDL_Rect& camera, const float& zoom)
{
	// This grid changes size based on the canvas and tile sizes. 
	auto xTiles = (mCanvasWidth / mTileSize);
	auto yTiles = (mCanvasHeight / mTileSize);

	// Set the grid colour to a light grey that is semi-transparent
	SDL_SetRenderDrawColor(renderer.get(), 70, 70, 70, 70);

	for (int i = 0; i < yTiles; i++)
	{
		for (int j = 0; j < xTiles; j++)
		{
			SDL_Rect newRect = { (j * mTileSize * zoom) - camera.x, (i * mTileSize * zoom) - camera.y, mTileSize * zoom, mTileSize * zoom };
			SDL_RenderDrawRect(renderer.get(), &newRect);
		}
	}
}

void RenderGuiSystem::CreateNewCanvas()
{

	mGridSnap = false;
	mCreateTiles = false;
	mCreateColliders = false;

	mTileSize = 64;
	mCanvasWidth = 640;
	mCanvasHeight = 448;

	// Kill all the current entities for new canvas
	for (auto& entity : GetSystemEntities())
	{
		entity.Kill();
	}
}

void RenderGuiSystem::ShowMouseLocationText(SDL_Rect& mouseBox, SDL_Rect& camera)
{
	// If the mouse is on the canvas and we are creating tiles/colliders, Display the mouse location
	if (!mMouseControl->MouseOutOfBounds() && (mCreateTiles || mCreateColliders))
	{
		// Update Grid values
		mGridX = static_cast<int>(mMouseControl->GetMousePosScreen().x) / mTileSize;
		mGridY = static_cast<int>(mMouseControl->GetMousePosScreen().y) / mTileSize;

		// Print the Current Grid number that we are in [x: 0, y: 0] 
		ImGui::TextColored(ImVec4(0, 255, 0, 1), "Grid [X: %d, Y: %d]", mGridX, mGridY);

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

		// If we are in gridsnap, print the location of the TL corner of the gridbox the mouse is in
		if (mGridSnap)
			ImGui::TextColored(ImVec4(0, 255, 0, 1), "Mouse [X: %d, Y: %d]", mGridX * mTileSize, mGridY * mTileSize);
		else // Print the location of the mouse relative to the canvas
			ImGui::TextColored(ImVec4(0, 255, 0, 1), "Mouse [X: %d, Y: %d]",
				static_cast<int>(mMouseControl->GetMousePosScreen().x), static_cast<int>(mMouseControl->GetMousePosScreen().y));
	}
}

const bool RenderGuiSystem::MouseOffCanvas() const
{
	if (mMouseControl->GetMousePosScreen().x > mCanvasWidth || mMouseControl->GetMousePosScreen().y > mCanvasHeight)
		return true;

	return false;
}
