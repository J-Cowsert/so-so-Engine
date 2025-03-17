#pragma once
#include "../Core/core.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"
#include <string>
#include <vector>


// Temporary code

namespace soso {

    class Mesh {
        
        struct MeshData {
            std::vector<float> TempVertices;
            std::vector<uint32_t> TempIndices;
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