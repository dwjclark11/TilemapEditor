#pragma once
#include "ECS/ECS.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "Utilities/Utilities.h"
#include <glm/glm.hpp>

class MouseControlSystem : public System
{
private:
	glm::vec2 mMouseRect;
	int mMousePosX;
	int mMousePosY;
	glm::vec2 mMousePosGrid;
	glm::vec2 mPrevMousePos;

	int mGridSize;

	bool mIsCollider;
	bool mGridSnap;
	bool mOverImGuiWindow;
	bool mLeftPressed;
	bool mRightPressed;

	SpriteComponent mSpriteComponent;
	TransformComponent mTransformComponent;


	// Private functions
private:
	void MouseBox(const std::unique_ptr<class AssetManager>& assetManager, std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& mouseBox, SDL_Rect& camera, bool collider = false);
public:
	MouseControlSystem();
	~MouseControlSystem() = default;

	void CreateTile(const std::unique_ptr<AssetManager>& assetManager, std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, struct SDL_Rect& mouseBox, struct SDL_Rect& camera, SDL_Event& event);
	void SetSpriteProperties(const std::string& assetID, int width, int height, int layer, int srcRectX, int srcRectY);
	void SetTransformScale(int scaleX, int scaleY);
	void SetMouseOverImGuiWindow(bool over) { mOverImGuiWindow = over; }

	const glm::vec2& GetMouseRect() const { return mMouseRect; }
	void SetMouseRect(int mouseRectX, int mouseRectY) { mMouseRect = glm::vec2(mouseRectX, mouseRectY); }
};