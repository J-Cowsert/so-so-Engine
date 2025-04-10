#pragma once
#include "../Core/core.h"

#include "so-so/Asset/Asset.h"
#include "Buffer.h" // Index Buffer and Vertex Buffer. Move them to their own classes
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

#include "glm/glm.hpp"
#include <filesystem>
#include <vector>

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Assimp {
	class Importer;
}

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

	class Submesh {
	public:
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
		uint32_t VertexCount;

		glm::mat4 Transform;
		glm::mat4 LocalTransform;

		std::string NodeName, MeshName;
	};

    class Mesh {

		friend class Renderer;

    public:
		Mesh() = default;
		Mesh(const std::filesystem::path& filepath);
		~Mesh();

		void Draw();

		std::vector<Submesh> GetSubmeshes() const { return m_Submeshes; }
		const std::vector<Vertex> GetVertices() const { return m_Vertices; }
		const std::vector<Index> GetIndices() const { return m_Indices; }
		std::shared_ptr<Shader> GetShader() const { return m_Shader; }

		const std::string& GetFilepath() const { return m_Filepath.string(); }


		void DumpBufferInfo();
	private:
		void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);

	private:
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::vector<Submesh> m_Submeshes;
		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;


		// Materials
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<Texture2D> m_DefaultTexture	;
		std::vector<std::shared_ptr<Texture2D>> m_Textures;
		std::vector<std::shared_ptr<Material>> m_Materials;

	private:
		std::unique_ptr<Assimp::Importer> m_Importer;
		const aiScene* m_aiScene;

		std::filesystem::path m_Filepath;
	};
}