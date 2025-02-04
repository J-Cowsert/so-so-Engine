#pragma once
#include "../Core/core.h"
#include "Buffer.h"
#include "VertexArray.h"
#include <string>
#include <vector>

namespace soso {

    class Model {
    public:
        Model(const std::string& path);
        void Draw();

    private:
        std::vector<float> m_Vertices; // Interleaved vertex data (position, normal, texcoord)
        std::vector<uint32_t> m_Indices; // Triangle indices
        std::shared_ptr<soso::VertexArray> m_VertexArray;
        std::shared_ptr<soso::VertexBuffer> m_VertexBuffer;
        std::shared_ptr<soso::IndexBuffer> m_IndexBuffer;
    };
}