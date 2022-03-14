#include "Application.h"
#include "AssetManager.h"

#include <SDL_ttf.h>

void Application::Init()
{
	// Init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		// TODO: Add error and logger
		return;
	}

	// Init SDL TTF
	if (TTF_Init() != 0)
	{
		// TODO: Add error and logger
		return;
	}

	// Use SDL to grab the displays resolution
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	// Create the window
	mWindow = std::unique_ptr<SDL_Window, SDLWindowDestroyer>(SDL_CreateWindow(
		"Tilemap Editor",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		mWindowWidth,
		mWindowHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	));

	if (!mWindow)
	{
		// TODO: create error and logger
		return;
	}

	// Create the renderer
	mRenderer = std::unique_ptr<SDL_Renderer, SDLRendererDestroyer>(SDL_CreateRenderer(
		mWindow.get(),
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	));

	if (!mRenderer)
	{
		// TODO: Error and LOGGER
		return;
	}
	SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);

	// Initialize ImGui context
	ImGui::CreateContext();
	ImGuiSDL::Initialize(mRenderer.get(), mWindowWidth, mWindowHeight);

	// Initialize Camera
	mCamera = {
		0, 0, mWindowWidth, mWindowHeight
	};

	// Initialize MouseBox
	mMouseBox.h = 1;
	mMouseBox.w = 1;

	// Initialize asset manager
	mAssetManager = std::make_unique<AssetManager>();
}

void Application::Draw()
{
	SDL_SetRenderDrawColor(mRenderer.get(), 0, 0, 0, 255);
	SDL_RenderClear(mRenderer.get());

	// Render Application stuff

	SDL_RenderPresent(mRenderer.get());
}

void Application::ProcessEvents()
{
	while (SDL_PollEvent(&mEvent))
	{
		// ImGui SDL Input
		ImGui_ImplSDL2_ProcessEvent(&mEvent);
		ImGuiIO& io = ImGui::GetIO();

		int mouseX, mouseY;
		const int mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);

		// Setup mouse inputs for ImGui
		io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		io.MouseDown[0] = mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		io.MouseDown[2] = mouseButtons & SDL_BUTTON(SDL_BUTTON_MIDDLE);

		// Handle Core SDL Events
		switch (mEvent.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		case SDL_KEYDOWN:
			if (mEvent.key.keysym.sym == SDLK_ESCAPE)
				mIsRunning = false;

			break;
		}
	}
}

void Application::Update()
{
	// TODO: Add logic to update!!
}

Application::Application()
	: mWindow(nullptr)
	, mRenderer(nullptr)
	, mCamera()
	, mMouseBox()
	, mIsRunning(true)
	, mDeltaTime(0.f)
	, mEvent()
	, mAssetManager(nullptr)
{

}

Application::~Application()
{
}

void Application::Run()
{
	Init();

	while (mIsRunning)
	{
		ProcessEvents();
		Update();
		Draw();
	}
}
