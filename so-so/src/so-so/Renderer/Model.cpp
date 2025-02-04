#include "sspch.h";

#include "Model.h"
#include "Renderer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace soso {


    Model::Model(const std::string& path) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
            SS_CORE_ERROR("Failed to load OBJ: {0}", err);
            return;
        }

        std::unordered_map<std::string, uint32_t> uniqueVertices;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                std::string key =
                    std::to_string(index.vertex_index) + "/" +
                    std::to_string(index.normal_index) + "/" +
                    std::to_string(index.texcoord_index);

                if (uniqueVertices.count(key)) {
                    m_Indices.push_back(uniqueVertices[key]);
                    continue;
                }

                uniqueVertices[key] = static_cast<uint32_t>(m_Vertices.size() / 8);

                // Position
                m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                m_Vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

                // Normal
                if (index.normal_index >= 0) {
                    m_Vertices.push_back(attrib.normals[3 * index.normal_index + 0]);
                    m_Vertices.push_back(attrib.normals[3 * index.normal_index + 1]);
                    m_Vertices.push_back(attrib.normals[3 * index.normal_index + 2]);
                }
                else {
                    m_Vertices.push_back(0.0f);
                    m_Vertices.push_back(0.0f);
                    m_Vertices.push_back(0.0f);
                }

                // Texture coordinate
                if (index.texcoord_index >= 0) {
                    m_Vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                    m_Vertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
                }
                else {
                    m_Vertices.push_back(0.0f);
                    m_Vertices.push_back(0.0f);
                }

                m_Indices.push_back(uniqueVertices[key]);
            }
        }

        if (attrib.normals.empty()) {
            SS_CORE_WARN("Generating smooth normals for {0}", path);
            std::vector<glm::vec3> normals(m_Vertices.size() / 8, glm::vec3(0.0f));

            // Compute face normals and accumulate
            for (size_t i = 0; i < m_Indices.size(); i += 3) {
                uint32_t i0 = m_Indices[i];
                uint32_t i1 = m_Indices[i + 1];
                uint32_t i2 = m_Indices[i + 2];

                glm::vec3 v0 = glm::vec3(
                    m_Vertices[i0 * 8 + 0],
                    m_Vertices[i0 * 8 + 1],
                    m_Vertices[i0 * 8 + 2]
                );
                glm::vec3 v1 = glm::vec3(
                    m_Vertices[i1 * 8 + 0],
                    m_Vertices[i1 * 8 + 1],
                    m_Vertices[i1 * 8 + 2]
                );
                glm::vec3 v2 = glm::vec3(
                    m_Vertices[i2 * 8 + 0],
                    m_Vertices[i2 * 8 + 1],
                    m_Vertices[i2 * 8 + 2]
                );

                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;
                glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

                normals[i0] += faceNormal;
                normals[i1] += faceNormal;
                normals[i2] += faceNormal;
            }

            // Normalize and update vertex normals
            for (size_t i = 0; i < normals.size(); i++) {
                glm::vec3 n = glm::normalize(normals[i]);
                m_Vertices[i * 8 + 3] = n.x;
                m_Vertices[i * 8 + 4] = n.y;
                m_Vertices[i * 8 + 5] = n.z;
            }
        }

        // Create buffers (same as before)
        m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(float));
        m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size());

        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoord" }
        };
        m_VertexBuffer->SetLayout(layout);

        m_VertexArray = VertexArray::Create();
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);
    }

    void Model::Draw() {
        if (m_VertexArray) 
            Renderer::Submit(m_VertexArray);
    }
}