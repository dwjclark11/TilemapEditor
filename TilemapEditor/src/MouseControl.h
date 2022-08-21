#pragma once
#include "Components/SpriteComponent.h"
#include "Components/TransformComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/AnimationComponent.h"
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

	bool mIsCollider, mIsAnimated;
	bool mGridSnap;
	bool mOverImGuiWindow;
	bool mLeftPressed;
	bool mRightPressed;
	bool mTileAdded;
	bool mTileRemoved;

	int mMostRecentTileId;

	// Components 
	SpriteComponent mSpriteComponent, mRemovedSpriteComponent;
	TransformComponent mTransformComponent, mRemovedTransformComponent;
	BoxColliderComponent mBoxColliderComponent, mRemovedBoxComponent;
	AnimationComponent mAnimationComponent, mRemovedAnimationComponent;


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

	/*
	Mouse Down functions
	Returns true if the left button is currently pressed.
	*/
	bool LeftButtonDown() { return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1); }
	bool MiddleButtonDown() { return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(2); }
	bool RightButtonDown() { return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(3); }

public:
	MouseControl();
	~MouseControl() = default;

	/*
	Creates a new tile on the canvas.
	@param - AssetManager_Ptr& is used to get the texture for the tile.
	@param - Renderer& this is a wrapper for the SDL_Renderer, this is needed to be passed into the Mousebox function
	@param - SDL_Rect& mousebox - the mouse box is the current size of the sprite you are tiling [16x16], [32x32] etc
	@param - SDL_Rect& camera - The main camera to offset the position of the tile in the 2D world
	@param - SDL_Event& event - This is used to know when the mouse was pressed
	*/
	void CreateTile(const AssetManager_Ptr& assetManager, Renderer& renderer,
		struct SDL_Rect& mouseBox, struct SDL_Rect& camera, union SDL_Event& event);

	/*
	*	Creates a Box Collideron the canvas.
	*	@param - AssetManager_Ptr, not really needed; however, the mousebox function uses the AssetManager for creating tiles.
	*	@param - Renderer& this is a wrapper for the SDL_Renderer, this is needed to be passed into the Mousebox function
	*	@param - SDL_Rect& mousebox - the mouse box is the current size of the sprite you are tiling [16x16], [32x32] etc
	*	@param - SDL_Rect& camera - The main camera to offset the position of the tile in the 2D world
	*	@param - SDL_Event& event - This is used to know when the mouse was pressed
	*/
	void CreateCollider(const AssetManager_Ptr& assetManager, Renderer& renderer,
		struct SDL_Rect& mouseBox, struct SDL_Rect& camera, union SDL_Event& event);

	/*
	*  This updates gets the position of the mouse from SDL and than updates the mouse based on the camera position
	*  and the current zoom value
	*/
	void UpdateMousePos(const SDL_Rect& camera);

	/*
	Set the Component Properties
	*/
	void SetSpriteProperties(const std::string& assetID, const int width, const int height, const int layer, const int srcRectX, const int srcRectY);
	void SetTransformScale(const int scaleX, const int scaleY);
	void SetBoxColliderProperties(const int width, const int height, const int offsetX, const int offsetY);
	void SetAnimationProperties(const int numFrames, const int frameSpeedRate, bool vertical, bool looped, int frameOffset);
	/*
	*  If the mouse is not currently overtop of the canvas, the mouse will be considered out of bounds and
	*  will not render the selected tile and will not allow you to create a tile in that area.
	*  If you want to place a tile beyond the canvas, increase the canvas size.
	*/
	const bool MouseOutOfBounds() const;

	/*
	*  The PanCamera function takes care of the calculations for panning the camera. Around. We want to be able to move the
	*  camera around with the mouse without causing coordinate problems for new tiles created.
	*  @params - SDL_Rect& camera - Takes in the main camera reference to change the camera position
	*  @params - AssetManager_Ptr and Renderer - Used to create the "HAND" texture when panning using the middle mouse button
	*/
	void PanCamera(SDL_Rect& camera, const float& dt, const AssetManager_Ptr& assetManager, Renderer& renderer);
	
	/*
	*	If the mouse is overtop of an ImGui window, we do not want to be able to set a tile or see the MouseBox.
	*/
	inline void SetMouseOverImGuiWindow(bool over) { mOverImGuiWindow = over; }
	
	/*
	*	Returns a glm::vec2 of the mouse rect position.
	*/
	inline const glm::vec2& GetMouseRect() const { return mMouseRect; }
	inline void SetMouseRect(int mouseRectX, int mouseRectY) { mMouseRect = glm::vec2(mouseRectX, mouseRectY); }

	/*
	*  SetGridSnap: This forces the tile to stay within the bounds of the current grid location that
	*  the mouse is currently hovering over. As the mouse leaves the current grid location, the tile
	*  will snap to the next location.
	*  @param - bool value to turn grid snap on/off
	*/
	inline void SetGridSnap(bool snap) { mGridSnap = snap; }

	/*
	*  SetGridSize will set the size that you want each square on the grid to be.
	*  @params - this takes in an intiger value and it sets the height and width of the grid.
	*/
	inline void SetGridSize(int size) { mGridSize = size; }


	inline void SetCollider(bool collider) { mIsCollider = collider; }
	inline void SetAnimated(bool animated) { mIsAnimated = animated; }
	inline const glm::vec2& GetMousePosScreen() const { return mMousePosScreen; }
	inline void UpdateZoom(const float& zoom) { mZoom = zoom; }
	inline void UpdateGridSize(const int& grid) { mGridSize = grid; }

	/*
	*	These functions are all for keeping track of what tiles were placed/removed ect. for the
	*	Undo/Redo commands.
	*/
	inline const bool& TileAdded() const { return mTileAdded; }
	inline void SetTileAdded(bool tile) { mTileAdded = tile; }
	inline const int& GetRecentTileId() const { return mMostRecentTileId; }

	inline const bool& TileRemoved() const { return mTileRemoved; }
	inline void SetTileRemoved(bool tile) { mTileRemoved = tile; }

	const BoxColliderComponent& GetRemovedBoxComponent() { return mRemovedBoxComponent; }
	const TransformComponent& GetRemovedTransform() { return mRemovedTransformComponent; }
	const SpriteComponent& GetRemovedSpriteComponent() { return mRemovedSpriteComponent; }
	const AnimationComponent& GetRemovedAnimationComponent() { return mRemovedAnimationComponent; }
};