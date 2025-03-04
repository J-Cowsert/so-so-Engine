#include "sspch.h";

#include "Mesh.h"
#include "Renderer.h"
#include <so-so/Asset/TextureImporter.h>


#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace soso {

    Mesh::Mesh(const std::string& path) {
        
        LoadMesh(path);
    }

    Mesh::~Mesh() {
       
    }

    void Mesh::LoadMesh(const std::string& path) {
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
                    m_MeshData.m_TempIndices.push_back(uniqueVertices[key]);
                    continue;
                }

                uniqueVertices[key] = static_cast<uint32_t>(m_MeshData.m_TempVertices.size() / 8);

                // Position
                m_MeshData.m_TempVertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
                m_MeshData.m_TempVertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
                m_MeshData.m_TempVertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

                // Normal
                if (index.normal_index >= 0) {
                    m_MeshData.m_TempVertices.push_back(attrib.normals[3 * index.normal_index + 0]);
                    m_MeshData.m_TempVertices.push_back(attrib.normals[3 * index.normal_index + 1]);
                    m_MeshData.m_TempVertices.push_back(attrib.normals[3 * index.normal_index + 2]);
                }
                else {
                    m_MeshData.m_TempVertices.push_back(0.0f);
                    m_MeshData.m_TempVertices.push_back(0.0f);
                    m_MeshData.m_TempVertices.push_back(0.0f);
                }

                if (index.texcoord_index >= 0) {
                    m_MeshData.m_TempVertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                    m_MeshData.m_TempVertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
                }
                else {
                    m_MeshData.m_TempVertices.push_back(0.0f);
                    m_MeshData.m_TempVertices.push_back(0.0f);
                }

                m_MeshData.m_TempIndices.push_back(uniqueVertices[key]);
            }
        }
    }
}