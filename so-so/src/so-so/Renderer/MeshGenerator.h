#pragma once

#include "Mesh.h"

namespace soso {

	class MeshGenerator {
	public:

		static std::shared_ptr<Mesh> GeneratePlane(float size, uint32_t resolution);
	};
}