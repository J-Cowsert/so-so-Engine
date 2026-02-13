#pragma once

#include "so-so/Core/Core.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "Math/AABB.h"

#include "glm/glm.hpp"
#include <filesystem>
#include <vector>


namespace soso {

	struct Vertex {

		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Bitangent;
		glm::vec2 TexCoord;
	};

	struct Index { // What if we don't want to render a triangle? TODO: investigate

		uint32_t V1, V2, V3;
	};

	static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

	struct Submesh { // TODO: Make more data oriented by splitting these into their own arrays instead of one big struct array

		glm::mat4 Transform;
		glm::mat4 LocalTransform;

		std::string NodeName, MeshName;
		
		AABB BoundingBox;
		
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
		uint32_t VertexCount;
	};

    class Mesh {
		friend class Renderer;
		friend class OpenGLRenderer;

    public:
		static std::shared_ptr<Mesh> Create(const std::filesystem::path& filepath);
		static std::shared_ptr<Mesh> Create(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, const glm::mat4& transform);

		explicit Mesh(const std::filesystem::path& filepath);
		Mesh(const std::vector<Vertex>& vertices, const std::vector<Index>&	 indices, const glm::mat4& transform);
		~Mesh();

		const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }
		const AABB& GetBoundingBox() const { return m_BoundingBox; }
		std::vector<std::shared_ptr<Material>> GetMaterials() const { return m_Materials; }

		std::string GetFilepath() const { return m_Filepath.string(); }

	private:
		void TraverseNodes(void* assimpNode, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);
		void DumpBufferInfo();

	private:
		std::vector<Submesh> m_Submeshes;
		std::vector<std::shared_ptr<Material>> m_Materials;

		AABB m_BoundingBox;

		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::filesystem::path m_Filepath;
	};

}
