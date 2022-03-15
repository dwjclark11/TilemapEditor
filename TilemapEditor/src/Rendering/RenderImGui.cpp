#include "RenderImGui.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_stdlib.h>
#include "ImGuiFunc.h"
#include "../Utilities/Utilities.h"
#include "../MouseControlSystem.h"
#include "../AssetManager.h"
#include <SDL.h>


RenderGuiSystem::RenderGuiSystem()
	: mCreateTiles(false)
	, mExit(false)
	, mCanvasWidth(640)
	, mCanvasHeight(480)
	, mTileSize(64)

{
	mImFuncs = std::make_unique<ImGuiFuncs>();
	mMouseControl = std::make_unique<MouseControlSystem>();

	mImFuncs->SetupImgui();
	mImFuncs->SetupImguiStyle();
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
			mImFuncs->ShowFileMenu();
			
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			mImFuncs->ShowToolsMenu(renderer, assetManager);
			ImGui::Checkbox("Create Tiles", &mCreateTiles);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (mCreateTiles)
	{
		mImFuncs->TileSetWindow(assetManager, renderer, mMouseControl->GetMouseRect());
		mImFuncs->ShowTileProperties(mMouseControl);
		mMouseControl->CreateTile(assetManager, renderer, mouseBox, camera, event);
	}

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
}

void RenderGuiSystem::RenderGrid(std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& camera)
{
	auto xTiles = mCanvasWidth / mTileSize;
	auto yTiles = mCanvasHeight / mTileSize;

	SDL_SetRenderDrawColor(renderer.get(), 70, 70, 70, 70);
	
	for (int i = 0; i < yTiles; i++)
	{
		for (int j = 0; j < xTiles; j++)
		{
			SDL_Rect newRect = { (j * mTileSize) - camera.x, (i * mTileSize) - camera.y, mTileSize, mTileSize};
			SDL_RenderDrawRect(renderer.get(), &newRect);
		}
	}
}
