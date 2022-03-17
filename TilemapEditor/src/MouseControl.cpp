#include "MouseControl.h"
#include "AssetManager.h"
#include <SDL.h>

void MouseControl::MouseBox(const std::unique_ptr<AssetManager>& assetManager, std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& mouseBox, SDL_Rect& camera, bool collider)
{
	// If Grid Snap is enabled, snap the tile to the next grid location
	if (mGridSnap)
	{
		mMousePosGrid.x = mMousePosX * mGridSize;
		mMousePosGrid.y = mMousePosY * mGridSize;

		if (mMousePosX >= 0) mMousePosGrid.x = mMousePosX / mGridSize;
		if (mMousePosY >= 0) mMousePosGrid.y = mMousePosY / mGridSize;

		mouseBox.x = (mMousePosGrid.x * mGridSize * mZoom) - camera.x;
		mouseBox.y = (mMousePosGrid.y * mGridSize * mZoom) - camera.y;
	}
	else // Float the center of the tile on the mouse
	{
		mouseBox.x = (mMousePosX * mZoom - camera.x - (mMouseRect.x * mTransformComponent.mScale.x) / 2);
		mouseBox.y = (mMousePosY * mZoom - camera.y - (mMouseRect.y * mTransformComponent.mScale.y) / 2);

	}

	// Do not draw the mouse box image outside of the mouse bounds
	if (MouseOutOfBounds())
		return;

	SDL_Rect srcRect = {
		mSpriteComponent.mSrcRect.x,
		mSpriteComponent.mSrcRect.y,
		mMouseRect.x,
		mMouseRect.y
	};

	SDL_Rect dstRect = {
		mouseBox.x,
		mouseBox.y,
		mouseBox.w * mMouseRect.x * mTransformComponent.mScale.x * mZoom,
		mouseBox.h * mMouseRect.y * mTransformComponent.mScale.y * mZoom
	};

	// If not a collider, draw the selected tile image
	if (!collider)
	{
		SDL_RenderCopyEx(
			renderer.get(),
			assetManager->GetTexture(mSpriteComponent.mAssetId).get(),
			&srcRect,
			&dstRect,
			mTransformComponent.mRotation,
			NULL,
			mSpriteComponent.mFlip
		);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer.get(), 255, 0, 0, 100);
		SDL_RenderFillRect(renderer.get(), &dstRect);
		SDL_RenderDrawRect(renderer.get(), &dstRect);

	}
}

bool MouseControl::FastTile(const glm::vec2& pos)
{
	if (mGridSnap)
	{
		if ((pos.x != mPrevMousePos.x || pos.y != mPrevMousePos.y) && LeftButtonDown())
			return true;
		else
			return false;
	}
	else
		return false;

}

MouseControl::MouseControl()
	: mMouseRect(glm::vec2(16, 16))
	, mMousePosX(0)
	, mMousePosY(0)
	, mMousePosGrid(glm::vec2(0))
	, mPrevMousePos(glm::vec2(mMousePosX, mMousePosY))
	, mMousePosScreen(glm::vec2(0))
	, mZoom(0)
	, mGridSize(16)
	, mPanX(0)
	, mPanY(0)
	, mIsCollider(false)
	, mGridSnap(true)
	, mOverImGuiWindow(false)
	, mLeftPressed(false)
	, mRightPressed(false)
	, mSpriteComponent()
	, mTransformComponent()
	, mBoxColliderComponent()
{

}

void MouseControl::CreateTile(const std::unique_ptr<AssetManager>& assetManager, std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& mouseBox, SDL_Rect& camera, SDL_Event& event)
{
	// Draw the Mouse Box Image, this follows the mouse
	MouseBox(assetManager, renderer, mouseBox, camera, false);

	// Do not create tiles outside of the mouse bounds
	if (MouseOutOfBounds())
		return;

	glm::vec2 pos = glm::vec2(
		mouseBox.x + camera.x / mGridSize,
		mouseBox.y + camera.y / mGridSize
	);
	// Set the transform position to the current mousebox position
	mTransformComponent.mPosition = glm::vec2(
		mouseBox.x + camera.x,
		mouseBox.y + camera.y
	);

	if (!LeftButtonDown())
		mLeftPressed = false;
	if (!RightButtonDown())
		mRightPressed = false;

	if ((event.type == SDL_MOUSEBUTTONDOWN || LeftButtonDown()) && !mOverImGuiWindow)
	{
		// If the left mouse button is pressed, create a tile/collider at that location
		if ((event.button.button == SDL_BUTTON_LEFT && !mLeftPressed) || FastTile(pos))
		{
			Entity tile = Registry::Instance().CreateEntity();
			tile.Group("tiles");
			tile.AddComponent<TransformComponent>(
				mTransformComponent.mPosition / glm::vec2(mZoom, mZoom),
				mTransformComponent.mScale,
				mTransformComponent.mRotation
				);

			tile.AddComponent<SpriteComponent>(
				mSpriteComponent.mAssetId,
				mSpriteComponent.mWidth,
				mSpriteComponent.mHeight,
				mSpriteComponent.mLayer,
				mSpriteComponent.mIsFixed,
				mSpriteComponent.mSrcRect.x,
				mSpriteComponent.mSrcRect.y,
				mSpriteComponent.mOffset
				);

			// If the tile is a box collider, Add a BoxColliderComponent
			if (mIsCollider)
			{
				tile.AddComponent<BoxColliderComponent>(
					mBoxColliderComponent.mHeight,
					mBoxColliderComponent.mWidth,
					mBoxColliderComponent.mOffset
					);
			}

			mLeftPressed = true;
			// This is used for Creating tiles faster
			mPrevMousePos.x = pos.x;
			mPrevMousePos.y = pos.y;
		}

		// If the right mouse button is pressed, remove the tile/collider at that location
		if (event.button.button == SDL_BUTTON_RIGHT && !mOverImGuiWindow && !mRightPressed)
		{
			if (!Registry::Instance().DoesGroupExist("tiles"))
				return;

			glm::vec2 subtract = glm::vec2(
				(mMouseRect.x * mTransformComponent.mScale.x) / 2,
				(mMouseRect.y * mTransformComponent.mScale.y) / 2
			);

			// Get all the entities from the group "tiles"
			auto entities = Registry::Instance().GetEntitiesByGroup("tiles");

			// Loop through tiles and remove the one that the mouse is hovering over
			for (auto& entity : entities)
			{
				auto& transform = entity.GetComponent<TransformComponent>();

				if (transform.mPosition.x <= mMousePosX - subtract.x + 15
					&& transform.mPosition.x >= mMousePosX - subtract.x - 15
					&& transform.mPosition.y <= mMousePosY - subtract.y + 15
					&& transform.mPosition.y >= mMousePosY - subtract.y - 15
					)
				{
					entity.Kill();
					mRightPressed = true;
					LOG_INFO("Tile with ID: {0} has been removed!", entity.GetID());
				}
			}
		}
	}
}

void MouseControl::CreateCollider(const std::unique_ptr<AssetManager>& assetManager, std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, SDL_Rect& mouseBox, SDL_Rect& camera, SDL_Event& event)
{
	MouseBox(assetManager, renderer, mouseBox, camera, true);

	// Do not create colliders outside of the mouse bounds
	if (MouseOutOfBounds())
		return;

	if (event.type == SDL_MOUSEBUTTONDOWN && !mLeftPressed)
	{
		if (event.button.button == SDL_BUTTON_LEFT && !mOverImGuiWindow)
		{
			Entity boxCollider = Registry::Instance().CreateEntity();
			boxCollider.Group("colliders");
			boxCollider.AddComponent<TransformComponent>(
				glm::vec2(
					mouseBox.x + camera.x,
					mouseBox.y + camera.y),
				mTransformComponent.mScale,
				mTransformComponent.mRotation
				);

			boxCollider.AddComponent<BoxColliderComponent>(
				mBoxColliderComponent.mHeight,
				mBoxColliderComponent.mWidth,
				mBoxColliderComponent.mOffset
				);
			mLeftPressed = true;
		}

		if (event.button.button == SDL_BUTTON_RIGHT && !mOverImGuiWindow)
		{
			glm::vec2 subtract = glm::vec2(
				(mMouseRect.x * mTransformComponent.mScale.x) / 2,
				(mMouseRect.y * mTransformComponent.mScale.y) / 2
			);

			// Get all the entities from the group "tiles"
			auto entities = Registry::Instance().GetEntitiesByGroup("colliders");

			// Loop through tiles and remove the one that the mouse is hovering over
			for (auto& entity : entities)
			{
				auto& transform = entity.GetComponent<TransformComponent>();

				if (transform.mPosition.x <= mMousePosX - subtract.x + 15
					&& transform.mPosition.x >= mMousePosX - subtract.x - 15
					&& transform.mPosition.y <= mMousePosY - subtract.y + 15
					&& transform.mPosition.y >= mMousePosY - subtract.y - 15
					)
				{
					entity.Kill();
					mRightPressed = true;
					LOG_INFO("Collider with ID: {0} has been removed!", entity.GetID());
				}
			}
		}
	}

	if (event.type == SDL_MOUSEBUTTONUP)
	{
		mLeftPressed = false;
		mRightPressed = false;
	}
}

void MouseControl::UpdateMousePos(const SDL_Rect& camera)
{
	// Get the location of the mouse from SDL
	SDL_GetMouseState(&mMousePosX, &mMousePosY);



	// Add the camera position to the mouse position
	mMousePosX += camera.x;
	mMousePosY += camera.y;

	mMousePosX /= mZoom;
	mMousePosY /= mZoom;

	// This value is used for Mouse Pos monitoring in the ImGui Main Bar
	mMousePosScreen.x = mMousePosX;
	mMousePosScreen.y = mMousePosY;
}

void MouseControl::SetSpriteProperties(const std::string& assetID, const int& width, const int& height, const int& layer, const int& srcRectX, const int& srcRectY)
{
	mSpriteComponent.mAssetId = assetID;
	mSpriteComponent.mWidth = width;
	mSpriteComponent.mHeight = height;
	mSpriteComponent.mLayer = layer;
	mSpriteComponent.mIsFixed = false;
	mSpriteComponent.mFlip = SDL_FLIP_NONE;
	mSpriteComponent.mSrcRect = { srcRectX, srcRectY, width, height };
}

void MouseControl::SetTransformScale(const int& scaleX, const int& scaleY)
{
	mTransformComponent.mScale = glm::vec2(scaleX, scaleY);
}

void MouseControl::SetBoxColliderProperties(const int& width, const int& height, const int& offsetX, const int& offsetY)
{
	mBoxColliderComponent.mWidth = width;
	mBoxColliderComponent.mHeight = height;
	mBoxColliderComponent.mOffset = glm::vec2(offsetX, offsetY);
}

const bool MouseControl::MouseOutOfBounds() const
{
	if (mMousePosScreen.x < 0 || mMousePosScreen.y < 0)
		return true;

	return false;
}

void MouseControl::PanCamera(SDL_Rect& camera, const float& dt)
{
	//LOG_INFO("Pan: [X:{0}, Y:{1}]", mPanX, mPanY);
	//LOG_INFO("Mouse: [X:{0}, Y:{1}]", mMousePosScreen.x, mMousePosScreen.y);
	//LOG_INFO("Mouse: [X:{0}, Y:{1}]", camera.x, camera.y);

	if (MiddleButtonDown())
	{
		if (mPanX != mMousePosScreen.x)
		{
			//glm::vec2 direction = glm::normalize(glm::vec2(mMousePosScreen.x, mMousePosScreen.y) - (glm::vec2(mPanX, mPanY)));

			camera.x -= (mMousePosScreen.x - mPanX) * mZoom * dt * 10;
			//mPanX = mMousePosScreen.x;
			LOG_INFO("X_Change: {0}", (mMousePosScreen.x - mPanX) * mZoom * dt);
		}

		if (mPanY != mMousePosScreen.y)
		{
			camera.y -= (mMousePosScreen.y - mPanY) * mZoom * dt * 10;
			//mPanY = mMousePosScreen.y;
			LOG_INFO("Y_Change: {0}", (mMousePosScreen.y - mPanY) * mZoom * dt);
		}
	}
	else
	{
		mPanX = mMousePosScreen.x;
		mPanY = mMousePosScreen.y;
	}
}
