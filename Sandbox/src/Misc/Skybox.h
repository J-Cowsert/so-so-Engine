#pragma once
#include <soso.h>
#include <so-so/Renderer/Renderer.h>
#include <so-so/Renderer/SceneCamera.h>

#include <so-so/Renderer/Texture.h>
#include <so-so/Asset/TextureImporter.h>


#include "glm/ext.hpp"

class Skybox {
public:
	Skybox() = default;

	Skybox(std::shared_ptr<soso::TextureCube> cubeTexture)
		: m_TexCube(cubeTexture) 
	{
		float vertices[] = {
			// positions
			-1.0f,  1.0f, -1.0f,  // 0
			-1.0f, -1.0f, -1.0f,  // 1
			 1.0f, -1.0f, -1.0f,  // 2
			 1.0f,  1.0f, -1.0f,  // 3
			-1.0f,  1.0f,  1.0f,  // 4
			-1.0f, -1.0f,  1.0f,  // 5
			 1.0f, -1.0f,  1.0f,  // 6
			 1.0f,  1.0f,  1.0f   // 7
		};
		uint32_t indices[] = {
			// Back face
			0, 1, 2,
			2, 3, 0,
			// Left face
			4, 5, 1,
			1, 0, 4,
			// Front face
			7, 6, 5,
			5, 4, 7,
			// Right face
			3, 2, 6,
			6, 7, 3,
			// Top face
			4, 0, 3,
			3, 7, 4,
			// Bottom face
			1, 5, 6,
			6, 2, 1
		};

		m_VA = soso::VertexArray::Create();
		m_VA->Bind();

		m_VB = soso::VertexBuffer::Create(vertices, sizeof(vertices));
		m_VB->SetLayout(soso::BufferLayout({
			{ soso::ShaderDataType::Float3, "a_Position" }
			}));
		m_VB->Bind();
		m_VA->AddVertexBuffer(m_VB);

		m_IB = soso::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_IB->Bind();
		m_VA->SetIndexBuffer(m_IB);

		m_Shader = soso::Shader::Create("assets/shaders/Skybox.glsl");
	}


	void Draw() {
		
		soso::RenderCommand::SetDepthFunction(soso::RenderCommand::DepthFunction::LEQUAL);

		m_Shader->Bind();
		m_TexCube->Bind(0);
		m_Shader->SetInt("u_Skybox", 0);

		soso::Renderer::SubmitSkybox(m_Shader, m_VA);

		soso::RenderCommand::SetDepthFunction(soso::RenderCommand::DepthFunction::LESS);
	}

private:

	std::shared_ptr<soso::VertexArray> m_VA;
	std::shared_ptr<soso::VertexBuffer> m_VB;
	std::shared_ptr<soso::IndexBuffer> m_IB;

	std::shared_ptr<soso::TextureCube> m_TexCube;
	std::shared_ptr<soso::Shader> m_Shader;
};