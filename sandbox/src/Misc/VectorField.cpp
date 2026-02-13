#include "VectorField.h"
#include "so-so/Renderer/Renderer.h"

void VectorField::GenerateGrid(int xCount, int yCount, int zCount, float spacing, const std::function<glm::vec3(const glm::vec3&)>& func) {

    m_Data.clear();
    m_Data.reserve(size_t(xCount) * yCount * zCount);

    // 1) First pass: sample raw vectors and track min/max length
    float minMag = std::numeric_limits<float>::infinity();
    float maxMag = -std::numeric_limits<float>::infinity();
    struct Raw { glm::vec3 pos, raw; float mag; };
    std::vector<Raw> rawSamples;
    rawSamples.reserve(m_Data.capacity());

    glm::vec3 offset((xCount - 1) * 0.5f,
        (yCount - 1) * 0.5f,
        (zCount - 1) * 0.5f);

    for (int z = 0; z < zCount; ++z) {
        for (int y = 0; y < yCount; ++y) {
            for (int x = 0; x < xCount; ++x) {
                glm::vec3 p = glm::vec3(x, y, z);
                p = (p - offset) * spacing;
                glm::vec3 v = func(p);
                float mag = glm::length(v);
                rawSamples.push_back({ p, v, mag });
                minMag = glm::min(minMag, mag);
                maxMag = glm::max(maxMag, mag);
            }
        }
    }

    float range = maxMag - minMag;

    // 2) Precompute final entries: normalized dir, color ramp, perp for head
    const glm::vec3 cold{ 0.0f, 0.5f, 1.0f };
    const glm::vec3 hot{ 1.0f, 0.2f, 0.0f };

    m_Data.reserve(rawSamples.size());
    for (auto& r : rawSamples) {
        Entry e;
        e.Position = r.pos;

        // normalize direction once
        if (r.mag > 1e-6f)
            e.DirNorm = r.raw / r.mag;
        else
            e.DirNorm = glm::vec3(1, 0, 0);

        e.DirNorm *= (spacing * 0.5f);

        // compute t in [0,1]
        float t = (range > 0.0f) ? (r.mag - minMag) / range : 0.0f;
        t = glm::clamp(t, 0.0f, 1.0f);
        // smoothstep for contrast
        t = glm::smoothstep(0.0f, 1.0f, t);

        // pre‐mix RGB
        e.Color = glm::mix(cold, hot, t);

        // precompute any perpendicular for arrow head
        glm::vec3 axis = (glm::abs(e.DirNorm.y) > 0.99f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
        e.Perp = glm::normalize(glm::cross(e.DirNorm, axis));

        m_Data.push_back(e);
    }

}

void VectorField::Draw(float scale) const {

    // 3 lines per entry: shaft + two head sides
    for (auto& e : m_Data) {
        glm::vec3 tip = e.Position + e.DirNorm * scale;
        glm::vec4 col(e.Color, 1.0f);

        // shaft
        soso::Renderer::SubmitLine(e.Position, tip, col);

        // arrow head
        float arrowLen = scale * 0.2f;
        glm::vec3 offL = -e.DirNorm * arrowLen + e.Perp * (arrowLen * 0.5f);
        glm::vec3 offR = -e.DirNorm * arrowLen - e.Perp * (arrowLen * 0.5f);

        soso::Renderer::SubmitLine(tip + offL, tip, col);
        soso::Renderer::SubmitLine(tip + offR, tip, col);
    }

}