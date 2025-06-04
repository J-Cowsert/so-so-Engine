#pragma once

#include "so-so/Renderer/Mesh.h"

#include "so-so/Core/ByteBuffer.h"

class Terrain {
public:
	Terrain(const std::filesystem::path& heightMapPath, float size, uint32_t resolution);
	~Terrain();

	Terrain(const Terrain&) = delete;
	Terrain& operator=(const Terrain&) = delete;

	std::shared_ptr<soso::Mesh> GetTerrainMesh() { return m_Terrain; }

private:
	struct HeightMap {

		soso::ByteBuffer ImageBuffer;
		uint32_t Width = 0, Height = 0;
	};

	HeightMap m_HeightMap;
	
	std::shared_ptr<soso::Mesh> m_Terrain;

};
