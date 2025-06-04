#include "Terrain.h"

#include "so-so/Resource/TextureImporter.h"

namespace Utils {

    float SampleHeightBilinear(const std::vector<float>& heights, uint32_t imgW, uint32_t imgH, float u, float v) {

        float fx = u * (imgW - 1);
        float fy = v * (imgH - 1);
        int x0 = static_cast<int>(std::floor(fx));
        int y0 = static_cast<int>(std::floor(fy));
        int x1 = std::min(x0 + 1, static_cast<int>(imgW - 1));
        int y1 = std::min(y0 + 1, static_cast<int>(imgH - 1));
        float tx = fx - x0;
        float ty = fy - y0;

        float h00 = heights[y0 * imgW + x0];
        float h10 = heights[y0 * imgW + x1];
        float h01 = heights[y1 * imgW + x0];
        float h11 = heights[y1 * imgW + x1];

        // lerp in x, then lerp those results in y
        float h0 = h00 + (h10 - h00) * tx;
        float h1 = h01 + (h11 - h01) * tx;
        return h0 + (h1 - h0) * ty;
    }
}

Terrain::Terrain(const std::filesystem::path& heightMapPath, float size, uint32_t resolution) {

    // Load RGBA8 image into ByteBuffer
    auto result = soso::TextureImporter::LoadImageFromFile(heightMapPath);

    m_HeightMap.ImageBuffer = result.Buffer;
    m_HeightMap.Width = result.Width;
    m_HeightMap.Height = result.Height;


    if (!m_HeightMap.ImageBuffer.Data) {
        SS_ASSERT(false, "Failed to load height-map image");
        return;
    }

    SS_ASSERT(m_HeightMap.Width != 0 && m_HeightMap.Height != 0, "");

    // Unpack the red channel into a float array [0,1]
    uint8_t* src = m_HeightMap.ImageBuffer.As<uint8_t>();
    uint32_t imgW = m_HeightMap.Width;
    uint32_t imgH = m_HeightMap.Height;
    uint32_t pixelCount = imgW * imgH;

    std::vector<float> heights;
    heights.reserve(pixelCount);

    for (uint32_t i = 0; i < pixelCount; i++) {
        float h = src[i * 4] / 255.0f; // Only the red channel
        heights.emplace_back(h);
    }

    // Prepare mesh containers
    resolution = glm::max(1, static_cast<int>(resolution));
    size_t vertsPerSide = static_cast<size_t>(resolution + 1);
    size_t vertexCount = vertsPerSide * vertsPerSide;
    size_t triangleCount = static_cast<size_t>(resolution) * resolution * 2;

    std::vector<soso::Vertex> vertices;
    vertices.reserve(vertexCount);
    std::vector<soso::Index> indices;
    indices.reserve(triangleCount);

    const float step = size / static_cast<float>(resolution);
    const float halfSize = size * 0.5f;
    const float invRes = 1.0f / static_cast<float>(resolution);

    // 
    const float heightScale = 10.0f;
    //

    // Generate vertices (positions, zeroed normals/tangents/bitangents, UVs)
    for (int i = 0; i <= resolution; i++) {
        for (int j = 0; j <= resolution; j++) {
            float u = static_cast<float>(i) * invRes;
            float v = static_cast<float>(j) * invRes;

            float h = Utils::SampleHeightBilinear(
                heights, imgW, imgH, u, v);
            h *= heightScale;

            float x = -halfSize + i * step;
            float z = -halfSize + j * step;

            soso::Vertex vert;
            vert.Position = { x, h, z };
            vert.Normal = { 0.0f, 1.0f, 0.0f };
            vert.Tangent = { 0.0f, 0.0f, 0.0f };
            vert.Bitangent = { 0.0f, 0.0f, 0.0f };
            vert.TexCoord = { u, v };

            vertices.emplace_back(vert);
        }
    }

    // Build index buffer
    int rowLen = resolution + 1;
    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            uint32_t v1 = i * rowLen + j;
            uint32_t v2 = (i + 1) * rowLen + j;
            uint32_t v3 = (i + 1) * rowLen + (j + 1);
            uint32_t v4 = i * rowLen + (j + 1);

            indices.emplace_back(soso::Index{ v1, v3, v2 });
            indices.emplace_back(soso::Index{ v1, v4, v3 });
        }
    }

    const float sobelX[3][3] = { {  1,  0, -1 },
                                 {  2,  0, -2 },
                                 {  1,  0, -1 } };

    const float sobelZ[3][3] = { {  1,  2,  1 },
                                 {  0,  0,  0 },
                                 { -1, -2, -1 } };

    auto H = [&](int ix, int iz) -> float
        {
            ix = std::clamp(ix, 0, int(resolution));
            iz = std::clamp(iz, 0, int(resolution));
            float u = float(ix) * invRes;
            float v = float(iz) * invRes;
            return Utils::SampleHeightBilinear(heights, imgW, imgH, u, v) * heightScale;
        };

    for (int i = 0; i <= resolution; ++i)
    {
        for (int j = 0; j <= resolution; ++j)
        {
            float gx = 0.0f, gz = 0.0f;
            for (int di = -1; di <= 1; ++di)
                for (int dj = -1; dj <= 1; ++dj)
                {
                    float h = H(i + di, j + dj);
                    gx += sobelX[di + 1][dj + 1] * h;
                    gz += sobelZ[di + 1][dj + 1] * h;
                }

            glm::vec3 n = glm::normalize(glm::vec3(-gx, 8.0f * step, -gz)); // 8 = Σ|kernel|
            vertices[i * (resolution + 1) + j].Normal = n;
        }
    }

    //--------------------------------------------------------------------
    // Analytic tangent / bitangent for a height-field grid
    //--------------------------------------------------------------------
    //  Tangent  = world +X projected onto the surface plane
    //  Bitangent = N × T  (right-handed)
    //--------------------------------------------------------------------
    const glm::vec3 WORLD_X(1.0f, 0.0f, 0.0f);

    for (auto& v : vertices)
    {
        // Project WORLD_X onto the plane orthogonal to N
        glm::vec3 T = WORLD_X - v.Normal * glm::dot(WORLD_X, v.Normal);
        float len = glm::length(T);
        if (len < 1e-6f)                     // extremely rare: normal almost parallel to X
            T = glm::cross(glm::vec3(0, 1, 0), v.Normal);   // pick any perpendicular axis
        else
            T /= len;

        glm::vec3 B = glm::normalize(glm::cross(v.Normal, T));

        v.Tangent = T;
        v.Bitangent = B;
    }
    
    m_Terrain = soso::Mesh::Create(vertices, indices, glm::mat4(1.0f));
}

Terrain::~Terrain() {

	if (m_HeightMap.ImageBuffer.Data) 
        m_HeightMap.ImageBuffer.Release();
}
