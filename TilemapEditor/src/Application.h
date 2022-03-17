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

	int mMsPrevFrame, mMsPerFrame;

	float mZoom;

	std::unique_ptr<class AssetManager> mAssetManager;

	// Define constants
	const int FPS = 60;
	const int MILLISECONDS_PER_FRAME = 1000 / FPS;

private:
	void Init();
	void Draw();
	void ProcessEvents();
	void Update();
	void UpdateDeltaTime();
	void CameraControl(SDL_Event& event);
	void Zoom(SDL_Event& event);
public:
	Application();
	~Application();

	void Run();
	void ShutDown();
};