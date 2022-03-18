#include "Application.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>
#include "Logger/Logger.h"
#include <SDL_ttf.h>
#include "Rendering/RenderSystem.h"
#include "Rendering/RenderCollisionSystem.h"
#include "Rendering/RenderImGui.h"

void Application::Init()
{
	// Init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		LOG_ERROR("APPLICATION__LINE__14: Failed to Initialize SDL: {0}", SDL_GetError());
		return;
	}

	// Init SDL TTF
	if (TTF_Init() != 0)
	{
		LOG_ERROR("APPLICATION__LINE__21: Failed to Initialize SDL_TTF!");
		return;
	}

	// Use SDL to grab the displays resolution
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	// Create the window
	mWindow = Window(SDL_CreateWindow(
		"Tilemap Editor",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	));

	if (!mWindow)
	{
		LOG_ERROR("APPLICATION__LINE__41: Failed to create window: {0}", SDL_GetError());
		return;
	}

	// Create the renderer
	mRenderer = Renderer(SDL_CreateRenderer(
		mWindow.get(),
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	));

	if (!mRenderer)
	{
		LOG_ERROR("APPLICATION__LINE__41: Failed to create renderer: {0}", SDL_GetError());
		return;
	}

	// Set the renderer to blend mode
	SDL_SetRenderDrawBlendMode(mRenderer.get(), SDL_BLENDMODE_BLEND);

	// Initialize ImGui context
	ImGui::CreateContext();
	ImGuiSDL::Initialize(mRenderer.get(), WINDOW_WIDTH, WINDOW_HEIGHT);

	// Initialize Camera --> This Centers the Grid
	mCamera = {
		DEFAULT_CAM_X, DEFAULT_CAM_Y, WINDOW_WIDTH, WINDOW_HEIGHT
	};

	// Initialize the mouse box for the tiles
	mMouseBox.x = 0;
	mMouseBox.y = 0;
	mMouseBox.h = 1;
	mMouseBox.w = 1;

	// Initialize asset manager
	mAssetManager = std::make_unique<AssetManager>();

	// Initialize the Logger
	Logger::Init();

	// Add all Necessary Systems to registry
	Registry::Instance().AddSystem<RenderSystem>();
	Registry::Instance().AddSystem<RenderCollisionSystem>();
	Registry::Instance().AddSystem<RenderGuiSystem>();
}

void Application::Draw()
{
	SDL_SetRenderDrawColor(mRenderer.get(), 0, 0, 0, 255);
	SDL_RenderClear(mRenderer.get());

	// Render Application Systems
	Registry::Instance().GetSystem<RenderSystem>().Update(mRenderer.get(), mAssetManager, mCamera, mZoom);
	Registry::Instance().GetSystem<RenderCollisionSystem>().Update(mRenderer, mCamera, mZoom);
	Registry::Instance().GetSystem<RenderGuiSystem>().RenderGrid(mRenderer, mCamera, mZoom);
	Registry::Instance().GetSystem<RenderGuiSystem>().Update(mAssetManager, mRenderer, mMouseBox, mCamera, mEvent, mZoom, mDeltaTime);

	/*
		This is a little hack to get SDL and ImGui to stop Ghosting!
		If ImGui is called Last, both will Ghost. The below is a small
		rectangle with the alpha set to 0 making it invisible.
	*/
	SDL_Rect rect = { 0, 0, 10, 10 };
	SDL_SetRenderDrawColor(mRenderer.get(), 255, 0, 0, 0);
	SDL_RenderFillRect(mRenderer.get(), &rect);
	SDL_RenderDrawRect(mRenderer.get(), &rect);


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
			CameraControl(mEvent);
			break;
		case SDL_MOUSEWHEEL:
			Zoom(mEvent);
		}
	}
}

void Application::Update()
{
	UpdateDeltaTime();
	Registry::Instance().Update();

	// Check for Exit
	if (Registry::Instance().GetSystem<RenderGuiSystem>().GetExit())
		mIsRunning = false;
}

void Application::UpdateDeltaTime()
{
	// If we are too fast, waste some time until we reach the desired time per frame.
	int timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - mMsPerFrame);

	if (timeToWait > 0 && timeToWait <= mMsPerFrame)
	{
		SDL_Delay(timeToWait);
	}

	// The difference in ticks since the last frame, converted to seconds
	mDeltaTime = (SDL_GetTicks() - mMsPrevFrame) / 1000.0f;

	// Store the current time frame
	mMsPrevFrame = SDL_GetTicks();
}

void Application::CameraControl(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
		case SDLK_w:
			mCamera.y -= 8;
			break;
		case SDLK_s:
			mCamera.y += 8;
			break;
		case SDLK_a:
			mCamera.x -= 8;
			break;
		case SDLK_d:
			mCamera.x += 8;
			break;
		case SDLK_SPACE: // Space Resets Zoom and pan to orginal values
			mZoom = DEFAULT_ZOOM;
			mCamera.x = DEFAULT_CAM_X;
			mCamera.y = DEFAULT_CAM_Y;
			break;
		}

		LOG_INFO("CAM [X:{0}, Y:{1}]", mCamera.x, mCamera.y);
	}
}

void Application::Zoom(SDL_Event& event)
{
	if (event.wheel.y > 0)
	{
		mZoom += 0.2f;
		//LOG_INFO("Zoom: {0}", mZoom);
	}
	else if (event.wheel.y < 0)
	{
		mZoom -= 0.2f;
		//LOG_INFO("Zoom: {0}", mZoom);

		if (mZoom <= 0.5)
			mZoom = 0.5;
	}

	// Zoom in the camera as well?
	mCamera.h* mZoom;
	mCamera.w* mZoom;
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
	, mMsPrevFrame(0)
	, mMsPerFrame(0)
	, mZoom(1)
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

void Application::ShutDown()
{
	// Shutdown ImGui
	ImGuiSDL::Deinitialize();
	ImGui::DestroyContext();

	// Shutdown SDL
	SDL_DestroyRenderer(mRenderer.get());
	SDL_DestroyWindow(mWindow.get());
	SDL_Quit();
}
