#pragma once
#include "../Core/core.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"
#include <string>
#include <vector>
#include <thread>
#include <atomic>

#include <glm/ext.hpp>


// Temporary slop code

namespace soso {

    class Mesh {
        
        struct MeshData {
            std::vector<float> m_TempVertices;
            std::vector<uint32_t> m_TempIndices;
        };

    public:
        Mesh() = default;
        Mesh(const std::string& path);
        ~Mesh();

        MeshData& GetMeshData() { return m_MeshData; }

    private:
        void LoadMesh(const std::string& path);

        MeshData m_MeshData;
    };

}