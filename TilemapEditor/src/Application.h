#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <memory>

class Application
{

private:
	struct SDLWindowDestroyer
	{
		void operator()(SDL_Window* window) const
		{
			SDL_DestroyWindow(window);
		}
	};

	struct SDLRendererDestroyer
	{
		void operator()(SDL_Renderer* renderer) const
		{
			SDL_DestroyRenderer(renderer);
		}
	};

private:
	const int mWindowWidth = 1920;
	const int mWindowHeight = 1024;
	
	std::unique_ptr<SDL_Window, SDLWindowDestroyer> mWindow;
	std::unique_ptr<SDL_Renderer, SDLRendererDestroyer> mRenderer;
	
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
};