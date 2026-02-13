#pragma once

#include <vector>
#include <functional>
#include "glm/glm.hpp"

namespace soso {
    class SceneCamera;
}

// Static Vector Field
class VectorField {
public:
    struct Entry {
        glm::vec3 Position;   // world‐space
        glm::vec3 DirNorm;    // normalized direction
        glm::vec3 Color;      // RGB ramp (0..1)
        glm::vec3 Perp;       // precomputed perpendicular for arrow head
    };

    void GenerateGrid(int xCount, int yCount, int zCount, float spacing, const std::function<glm::vec3(const glm::vec3&)>& func);

    void Draw(float scale = 1.0f) const;

    size_t GetCount() const { return m_Data.size(); }

private:
    std::vector<Entry> m_Data;
};