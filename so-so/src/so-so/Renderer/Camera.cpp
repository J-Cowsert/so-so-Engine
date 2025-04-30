#include "sspch.h"
#include "Camera.h"

soso::Camera::Camera(const glm::mat4& projection)
	: m_Projection(projection) 
{
}

soso::Camera::Camera(const float degreeFOV, const float width, const float height, const float nearP, const float farP)
	: m_Projection(glm::perspectiveFov(glm::radians(degreeFOV), width, height, nearP, farP)) 
{
}
