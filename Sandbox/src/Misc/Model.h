
#include "soso.h"

#include <so-so/Renderer/Renderer.h>
#include <so-so/Renderer/Texture.h>
#include <so-so/Renderer/Mesh.h>
#include <so-so/Asset/TextureImporter.h>
#include "glm/ext.hpp"


struct Transform {
	glm::vec3 Position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale{ 1.0f, 1.0f, 1.0f };

	glm::mat4 GetMatrix() const {
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, Position);
		matrix = glm::rotate(matrix, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
		matrix = glm::rotate(matrix, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
		matrix = glm::rotate(matrix, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
		matrix = glm::scale(matrix, Scale);
		return matrix;
	}
};


class Model {
public:

	Model(std::shared_ptr<soso::Shader> shader, const soso::Mesh& mesh, std::shared_ptr<soso::Texture2D> texture) 
		: m_Shader(shader), m_Texture(texture)
	{
		m_Mesh = mesh;
		auto meshData = m_Mesh.GetMeshData();

		//SS_TRACE("Loaded mesh: {0} vertices, {1} indices",
		//	meshData.m_TempVertices.size() / sizeof(float),
		//	meshData.m_TempIndices.size()
		//);

		m_VA = soso::VertexArray::Create();
		m_VA->Bind();

		//m_Shader = soso::Shader::Create("assets/shaders/Phong.glsl");
		

		m_IB = soso::IndexBuffer::Create(meshData.TempIndices.data(), (uint32_t)meshData.TempIndices.size());
		m_VB = soso::VertexBuffer::Create(meshData.TempVertices.data(), (uint32_t)meshData.TempVertices.size() * sizeof(float));

		m_VB->Bind();
		m_VB->SetLayout(soso::BufferLayout({
				{ soso::ShaderDataType::Float3, "a_Position" },
				{ soso::ShaderDataType::Float3, "a_Normal" },
				{ soso::ShaderDataType::Float2, "a_TexCoord"}
		}));


		m_VA->AddVertexBuffer(m_VB);
		m_VA->SetIndexBuffer(m_IB);

		m_Transform.Scale *= 15;
		
	}

	void SetMesh(const soso::Mesh& mesh) { m_Mesh = mesh; }
	void SetShader(const std::shared_ptr<soso::Shader>& shader) { m_Shader = shader; }
	void SetTexture(const std::shared_ptr<soso::Texture2D>& texture) { m_Texture = texture; }
	void SetTransform(const Transform& transform) { m_Transform = transform; }

	Transform& GetTransform() { return m_Transform; }

	void Draw() const {

		m_Texture->Bind(0);
		m_Shader->Bind();
		m_Shader->SetInt("u_Texture", 0);

		soso::Renderer::Submit(m_Shader, m_VA, m_Transform.GetMatrix());
	}

private:
	soso::Mesh m_Mesh;

	std::shared_ptr<soso::VertexArray> m_VA;
	std::shared_ptr<soso::VertexBuffer> m_VB;
	std::shared_ptr<soso::IndexBuffer> m_IB;

	Transform m_Transform;
	std::shared_ptr<soso::Shader> m_Shader;
	std::shared_ptr<soso::Texture2D> m_Texture;
};
