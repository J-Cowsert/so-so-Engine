#pragma once
#include "so-so/Core/Core.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

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

	struct Index {

		uint32_t V1, V2, V3;
	};

	static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

	struct Submesh {

		glm::mat4 Transform;
		glm::mat4 LocalTransform;
		
		std::string NodeName, MeshName;
		
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
		Mesh(const std::vector<Vertex>& vertices, const std::vector<Index> indices, const glm::mat4& transform);
		~Mesh();

		const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }
		const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
		const std::vector<Index>& GetIndices() const { return m_Indices; }
		std::shared_ptr<Shader> GetShader() const { return m_DefaultShader; }
		std::vector<std::shared_ptr<Material>> GetMaterials() const { return m_Materials; }

		const std::string& GetFilepath() const { return m_Filepath.string(); }

		void DumpBufferInfo();

	private:
		void TraverseNodes(void* assimpNode, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);

	private:
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::vector<Submesh> m_Submeshes;
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;

		// Materials
		std::shared_ptr<Shader> m_DefaultShader;
		std::vector<std::shared_ptr<Material>> m_Materials;

	private:
		std::filesystem::path m_Filepath;
	};
}