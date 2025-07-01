#pragma once

#include "glm/glm.hpp"

namespace soso {

	struct AABB {

		glm::vec3 Min, Max;

		AABB() : Min(0), Max(0) {}

		AABB(glm::vec3 min, glm::vec3 max)
			: Min(min), Max(max) 
		{
		}

		glm::vec3 Center() { return (Min + Max) * 0.5f; }
	};
}