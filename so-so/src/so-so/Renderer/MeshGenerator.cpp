#include "sspch.h"
#include "MeshGenerator.h"

namespace soso {

    std::shared_ptr<Mesh> MeshGenerator::GeneratePlane(float size, int resolution) {

        std::vector<Vertex> vertices;
        std::vector<Index> indices;

        resolution = glm::max(1, resolution);

        const float step = size / static_cast<float>(resolution);
        const float startPos = -size * 0.5f;
        const int rowLen = resolution + 1;
 
        float currX, currZ;

        // Vertices
        for (int i = 0; i <= resolution; i++) {
            for (int j = 0; j <= resolution; j++) {

                currX = startPos + i * step;
                currZ = startPos + j * step;

                Vertex& vertex = vertices.emplace_back();
                vertex.Position  = { currX, 0, currZ };
                vertex.Normal    = { 0, 1, 0 };
                vertex.Tangent   = { 1, 0, 0 };
                vertex.Bitangent = { 0, 0, 1 };
                vertex.TexCoord  = {
                    static_cast<float>(i) / resolution,
                    static_cast<float>(j) / resolution
                };
            }
        }

        // Indices
        for (int i = 0; i < resolution; i++) {
            for (int j = 0; j < resolution; j++) {

                uint32_t v1 = i * rowLen + j;
                uint32_t v2 = (i + 1) * rowLen + j;
                uint32_t v3 = (i + 1) * rowLen + (j + 1);
                uint32_t v4 = i * rowLen + (j + 1);

                // CCW winding order
                indices.emplace_back(Index{ v1, v3, v2 });
                indices.emplace_back(Index{ v1, v4, v3 });
            }
        }

        return std::make_shared<Mesh>(vertices, indices, glm::mat4(1.0));
    }
}