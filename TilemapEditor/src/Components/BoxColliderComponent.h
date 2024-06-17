#pragma once
#include <glm/glm.hpp>

struct BoxColliderComponent
{
	int mWidth;
	int mHeight;
	glm::vec2 mOffset;

	BoxColliderComponent( int width = 0, int height = 0, glm::vec2 offset = glm::vec2( 0 ) )
	{
		mWidth = width;
		mHeight = height;
		mOffset = offset;
	}
};