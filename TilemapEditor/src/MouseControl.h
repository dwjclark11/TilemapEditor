#pragma once
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "Components/BoxColliderComponent.h"
#include "AssetManager.h"
#include "Utilities/Utilities.h"
#include "ECS/ECS.h"
#include <glm/glm.hpp>

class MouseControl
{
private:
	glm::vec2 mMouseRect;
	int mMousePosX;
	int mMousePosY;
	glm::vec2 mMousePosGrid;
	glm::vec2 mPrevMousePos;
	glm::vec2 mMousePosScreen;

	float mZoom;
	int mGridSize, mPanX, mPanY;

	bool mIsCollider;
	bool mGridSnap;
	bool mOverImGuiWindow;
	bool mLeftPressed;
	bool mRightPressed;

	// Components 
	SpriteComponent mSpriteComponent;
	TransformComponent mTransformComponent;
	BoxColliderComponent mBoxColliderComponent;

	// +/- tolerance for tile removal
	const int TOLERANCE = 15;

	// Private functions
private:
	void MouseBox(const AssetManager_Ptr& assetManager, Renderer& renderer,
		SDL_Rect& mouseBox, SDL_Rect& camera, bool collider = false);

	/*
		Fast Tile: This is for quickly placing tiles while using gridsnap mode.
		If the current mouse position not equal to the previous recorded position,
		place a new tile.
	*/
	bool FastTile(const glm::vec2& pos);

	// Mouse Buttons Down Functions
	bool LeftButtonDown() { return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1); }
	bool MiddleButtonDown() { return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(2); }
	bool RightButtonDown() { return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(3); }

public:
	MouseControl();
	~MouseControl() = default;

	void CreateTile(const AssetManager_Ptr& assetManager, Renderer& renderer,
		struct SDL_Rect& mouseBox, struct SDL_Rect& camera, union SDL_Event& event);
	void CreateCollider(const AssetManager_Ptr& assetManager, Renderer& renderer,
		struct SDL_Rect& mouseBox, struct SDL_Rect& camera, union SDL_Event& event);

	void UpdateMousePos(const SDL_Rect& camera);

	// Set the Component Properties
	void SetSpriteProperties(const std::string& assetID, const int& width, const int& height, const int& layer, const int& srcRectX, const int& srcRectY);
	void SetTransformScale(const int& scaleX, const int& scaleY);
	void SetBoxColliderProperties(const int& width, const int& height, const int& offsetX, const int& offsetY);

	const bool MouseOutOfBounds() const;

	void PanCamera(SDL_Rect& camera, const float& dt);
	inline void SetMouseOverImGuiWindow(bool over) { mOverImGuiWindow = over; }
	inline const glm::vec2& GetMouseRect() const { return mMouseRect; }
	inline void SetMouseRect(int mouseRectX, int mouseRectY) { mMouseRect = glm::vec2(mouseRectX, mouseRectY); }

	inline void SetGridSnap(bool snap) { mGridSnap = snap; }
	inline void SetGridSize(int size) { mGridSize = size; }
	inline void SetCollider(bool collider) { mIsCollider = collider; }
	inline const glm::vec2& GetMousePosScreen() const { return mMousePosScreen; }
	inline void UpdateZoom(const float& zoom) { mZoom = zoom; }
	inline void UpdateGridSize(const int& grid) { mGridSize = grid; }
};