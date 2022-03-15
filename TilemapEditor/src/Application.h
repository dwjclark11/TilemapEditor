#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <memory>
#include "./Utilities/Utilities.h"

class Application
{

private:


private:
	const int mWindowWidth = 1366;
	const int mWindowHeight = 768;

	std::unique_ptr<SDL_Window, Util::SDLDestroyer> mWindow;
	std::unique_ptr<SDL_Renderer, Util::SDLDestroyer> mRenderer;

	SDL_Rect mCamera;
	SDL_Rect mMouseBox;
	SDL_Event mEvent;

	float mDeltaTime;
	bool mIsRunning;

	std::unique_ptr<class AssetManager> mAssetManager;

private:
	void Init();
	void Draw();
	void ProcessEvents();
	void Update();

public:
	Application();
	~Application();

	void Run();
	void ShutDown();
};