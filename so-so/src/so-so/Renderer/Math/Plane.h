#pragma once

#include "glm/glm.hpp"

namespace soso {

	struct Plane {

		glm::vec3 Normal;
		float Distance;

		Plane() : Normal(0), Distance(0) {}

		Plane(glm::vec3 normal, float distance)
			: Normal(normal), Distance(distance)
		{
		}
	};
}