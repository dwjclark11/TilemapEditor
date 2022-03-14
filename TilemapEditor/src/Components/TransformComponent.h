#pragma once
#include <glm/glm.hpp>

struct TransformComponent
{
	glm::vec2 mPosition;
	glm::vec2 mScale;
	double mRotation;

	TransformComponent(glm::vec2 position = glm::vec2(0), glm::vec2 scale = glm::vec2(1, 1), double rotation = 0.0f)
	{
		mPosition = position;
		mScale = scale;
		mRotation = rotation;
	}
};