#pragma once
#include "ECS/ECS.h"
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Utilities/Utilities.h"
#include <glm/glm.hpp>

class MouseControl 
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
	BoxColliderComponent mBoxColliderComponent;


	// Private functions
private:
	void MouseBox(const std::unique_ptr<class AssetManager>& assetManager, std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& mouseBox, SDL_Rect& camera, bool collider = false);
public:
	MouseControl();
	~MouseControl() = default;

	void CreateTile(const std::unique_ptr<AssetManager>& assetManager, std::unique_ptr<struct SDL_Renderer, Util::SDLDestroyer>& renderer, struct SDL_Rect& mouseBox, struct SDL_Rect& camera, SDL_Event& event);
	void SetSpriteProperties(const std::string& assetID, const int& width, const int& height, const int& layer, const int& srcRectX, const int& srcRectY);
	void SetTransformScale(const int& scaleX, const int& scaleY);
	void SetBoxColliderProperties(const int& width, const int& height, const int& offsetX, const int& offsetY);
	
	inline void SetMouseOverImGuiWindow(bool over) { mOverImGuiWindow = over; }
	inline const glm::vec2& GetMouseRect() const { return mMouseRect; }
	inline void SetMouseRect(int mouseRectX, int mouseRectY) { mMouseRect = glm::vec2(mouseRectX, mouseRectY); }

	inline void SetGridSnap(bool snap) { mGridSnap = snap; }
	inline void SetGridSize(int size) { mGridSize = size; }
	inline void SetCollider(bool collider) { mIsCollider = collider; }
};