#include "sspch.h";
#include "Mesh.h"
#include "so-so/Renderer/Renderer.h"

#include "so-so/Asset/TextureImporter.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>


// Temporary
#include "glad/glad.h"

#define MESH_DEBUG_LOG true // Toggle for debug logging

#ifdef MESH_DEBUG_LOG
	#define MESH_DEBUG(x, ...) SS_CORE_TRACE(__VA_ARGS__)
#else
	#define MESH_DEBUG(x, ...)
#endif

namespace soso {

	struct LogStream : public Assimp::LogStream {

		static void Initialize() {

			if (Assimp::DefaultLogger::isNullLogger()) {
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
			}
		}

		void write(const char* message) override {
			SS_CORE_WARN("Assimp: {0}", message);
		}
	};

	glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix) {
		glm::mat4 result;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
		result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
		result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
		result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
		return result;
	}

	static const uint32_t s_MeshImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_Triangulate |             // Triangulate all faces of all meshes
		aiProcess_SortByPType |             // Split meshes with more than one primitive type in homogeneous sub-meshes
		aiProcess_GenNormals |              // Generate smooth normals for all vertices in the mesh if they dont exist
		aiProcess_GenUVCoords |             // convert non-UV mappings to proper texture coordinate channels
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_ValidateDataStructure;    // This makes sure that all indices are valid, all material references are correct, etc

	Mesh::Mesh(const std::filesystem::path& filepath)
		: m_Filepath(filepath) {

		LogStream::Initialize();

		SS_CORE_INFO("Loading Mesh: {0}", filepath);

		m_Shader = Renderer::GetShaderLibrary()->Get("Texture");

		m_Importer = std::make_unique<Assimp::Importer>();
		const aiScene* scene = m_Importer->ReadFile(filepath.generic_string(), s_MeshImportFlags);

		if (!scene || !scene->HasMeshes()) {
			SS_CORE_ERROR("Filed to load mesh file: {0}", filepath);
			return;
		}

		m_aiScene = scene;

		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		m_Submeshes.reserve(scene->mNumMeshes);

		// Meshes
		for (size_t i = 0; i < scene->mNumMeshes; i++) {

			aiMesh* mesh = scene->mMeshes[i];

			Submesh& submesh = m_Submeshes.emplace_back();
			submesh.BaseVertex = vertexCount;
			submesh.BaseIndex = indexCount;
			submesh.MaterialIndex = mesh->mMaterialIndex;
			submesh.IndexCount = mesh->mNumFaces * 3;
			submesh.VertexCount = mesh->mNumVertices;
			submesh.MeshName = mesh->mName.C_Str();

			vertexCount += submesh.VertexCount;
			indexCount += submesh.IndexCount;

			SS_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
			SS_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

			// Vertices
			for (size_t j = 0; j < mesh->mNumVertices; j++) {

				Vertex vertex;
				vertex.Position = { mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z };
				vertex.Normal = { mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z };

				if (mesh->HasTangentsAndBitangents()) {
					vertex.Tangent = { mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z };
					vertex.Bitangent = { mesh->mBitangents[j].x, mesh->mBitangents[j].y, mesh->mBitangents[j].z };
				}

				if (mesh->HasTextureCoords(0))
					vertex.TexCoord = { mesh->mTextureCoords[0][j].x, 1 - mesh->mTextureCoords[0][j].y };

				m_Vertices.push_back(vertex);
			}

			// Indices
			for (size_t j = 0; j < mesh->mNumFaces; j++) {

				SS_CORE_ASSERT((mesh->mFaces[j].mNumIndices == 3), "Mesh face is not a triangle");
				Index index;
				index = { mesh->mFaces[j].mIndices[0], mesh->mFaces[j].mIndices[1], mesh->mFaces[j].mIndices[2] };
				m_Indices.push_back(index);
			}
		}

		TraverseNodes(scene->mRootNode);

		// Materials
		if (scene->HasMaterials()) {

			SS_CORE_INFO("---- Materials - {0} ----", filepath);

			m_Textures.resize(scene->mNumTextures);
			m_Materials.resize(scene->mNumMaterials);

			for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
				
				auto aiMaterial = scene->mMaterials[i];
				aiString name;

				aiMaterial->Get(AI_MATKEY_NAME, name);
				//SS_CORE_INFO("Mat name: {0}", name.data);
				
				// Will change once Material developes
				auto mat = Material::Create(m_Shader);
				m_Materials[i] = mat;


				float shininess;
				if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS) {
					shininess = 80.0;
				}
				mat->SetShininess(shininess);

				aiString aiTexPath;
				bool hasDiffuseMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == aiReturn_SUCCESS;

				if (hasDiffuseMap) {
					SS_CORE_INFO("tex name: {0}", std::string(aiTexPath.data));
					std::filesystem::path path = filepath;
					auto parentPath = path.parent_path();
					SS_CORE_INFO("Parent path: {0}", parentPath);
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();
					SS_CORE_INFO("diffuse map path: {0}", texturePath);

					auto texture = TextureImporter::LoadTexture2D(texturePath);

					if (texture->IsLoaded()) {
						//m_Textures[i] = texture;
						mat->SetDiffuseMap(texture);
					}
				}

				bool hasSpecularMap = aiMaterial->GetTexture(aiTextureType_SPECULAR, 0, &aiTexPath) == aiReturn_SUCCESS;

				if (hasSpecularMap) {
					std::filesystem::path path = filepath;
					auto parentPath = path.parent_path();
					SS_CORE_INFO("Parent path: {0}", parentPath);
					parentPath /= std::string(aiTexPath.data);
					std::string texturePath = parentPath.string();
					SS_CORE_INFO("specular map path: {0}", texturePath);

					auto texture = TextureImporter::LoadTexture2D(texturePath);
					
					if (texture->IsLoaded()) {
						//m_Textures[i] = texture;
						mat->SetSpecularMap(texture);
					}
				}
			}
		}
	
		m_VertexArray = VertexArray::Create();
		m_VertexArray->Bind();

		BufferLayout vertexLayout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float3, "a_Normal" },
				{ ShaderDataType::Float3, "a_Tangent" },
				{ ShaderDataType::Float3, "a_Bitangent" },
				{ ShaderDataType::Float2, "a_TexCoord" },
		};


		m_VertexBuffer = VertexBuffer::Create(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
		m_VertexBuffer->SetLayout(vertexLayout);

		m_IndexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		//m_DefaultTexture = TextureImporter::LoadTexture2D("assets/textures/wood.png");
	}

	Mesh::~Mesh() {}

	void Mesh::Draw() {

			
	}

	void Mesh::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level) {

		glm::mat4 localTransform = Mat4FromAssimpMat4(node->mTransformation);
		glm::mat4 transform = parentTransform * localTransform; // Position relative to the parant node's transform

		for (uint32_t i = 0; i < node->mNumMeshes; i++) {

			uint32_t meshIdx = node->mMeshes[i];
			auto& submesh = m_Submeshes[meshIdx];
			submesh.LocalTransform = localTransform;
			submesh.Transform = transform;
			submesh.NodeName = node->mName.C_Str();
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			TraverseNodes(node->mChildren[i], transform, level + 1);
		}

	}

	void Mesh::DumpBufferInfo() {

		std::cout << "\n";
		SS_CORE_TRACE("============ MESH LOG - Buffer Info ============");
		SS_CORE_TRACE("INDEX BUFFER INFO: size: {0}. count: {1}", m_IndexBuffer->GetSize(), m_IndexBuffer->GetCount());
		SS_CORE_TRACE("VERTEX BUFFER INFO: size: {0}. count: {1}");
		SS_CORE_TRACE("VERTEX ARRAY BUFFER INFO: size: {0}. count: {1}");
		SS_CORE_TRACE("Indicies Buffer Data: count: {0}. size: {1}", m_Indices.size(), m_Vertices.size() * sizeof(Index));
		SS_CORE_TRACE("Verticies Buffer Data: count: {0}. size: {1}", m_Vertices.size(), m_Vertices.size() * sizeof(Vertex));
		SS_CORE_TRACE("------------------");
		SS_CORE_TRACE("Mesh has {0} submeshes, {1} materials, {2} textures", m_Submeshes.size(), m_Materials.size(), m_Textures.size());

		float minU = 1.0f, maxU = 0.0f, minV = 1.0f, maxV = 0.0f;
		for (const auto& vertex : m_Vertices) {
			minU = std::min(minU, vertex.TexCoord.x);
			maxU = std::max(maxU, vertex.TexCoord.x);
			minV = std::min(minV, vertex.TexCoord.y);
			maxV = std::max(maxV, vertex.TexCoord.y);
		}
		SS_CORE_TRACE("UV range: U [{}, {}], V [{}, {}]", minU, maxU, minV, maxV);

#if 0
		uint32_t idx = 0;
		for (Submesh& submesh : m_Submeshes) {

			SS_CORE_INFO("index {0}", idx);
			SS_CORE_INFO("Index Count: {0}. BaseIndex: {1}. BaseVertex: {2}.", submesh.IndexCount, submesh.BaseIndex, submesh.BaseVertex);
			SS_CORE_INFO("Offset into Index Buffer: {0}", (sizeof(uint32_t) * submesh.BaseIndex));

			idx++;
		}

		for (size_t i = 0; i < m_Vertices.size(); i++) {
			auto& vertex = m_Vertices[i];
			SS_CORE_TRACE("Vertex: {0}", i);
			SS_CORE_TRACE("Position: {0}, {1}, {2}", vertex.Position.x, vertex.Position.y, vertex.Position.z);
			SS_CORE_TRACE("Normal: {0}, {1}, {2}", vertex.Normal.x, vertex.Normal.y, vertex.Normal.z);
			SS_CORE_TRACE("Bitangent: {0}, {1}, {2}", vertex.Bitangent.x, vertex.Bitangent.y, vertex.Bitangent.z);
			SS_CORE_TRACE("Tangent: {0}, {1}, {2}", vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z);
			SS_CORE_TRACE("TexCoord: {0}, {1}", vertex.TexCoord.x, vertex.TexCoord.y);
		}
#endif
		SS_CORE_TRACE("================================================\n");
	}
}