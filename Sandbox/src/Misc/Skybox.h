#pragma once
#include <soso.h>
#include <so-so/Renderer/Renderer.h>





#include "glm/ext.hpp"

struct Skybox {

	std::shared_ptr<soso::VertexArray> m_VA;
	std::shared_ptr<soso::VertexBuffer> m_VB;
	std::shared_ptr<soso::IndexBuffer> m_IB;

	std::shared_ptr<soso::TextureCube> m_TexCube;
	std::shared_ptr<soso::Shader> m_Shader;

	Skybox() = default;

	explicit Skybox(std::shared_ptr<soso::TextureCube> cubeTexture)
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

		
		m_Shader = soso::Renderer::GetShaderLibrary()->Get("Skybox");

		m_VA = soso::VertexArray::Create();
		m_VA->Bind();

		m_VB = soso::VertexBuffer::Create(vertices, sizeof(vertices));
		m_VB->SetLayout(soso::VertexBufferLayout({
			{ soso::ShaderDataType::Float3, "a_Position" }
			}));
		
		m_VA->AddVertexBuffer(m_VB);

		m_IB = soso::IndexBuffer::Create(indices, sizeof(indices));
		
		m_VA->SetIndexBuffer(m_IB);
		m_VA->Unbind();
	}


	void Draw() {
		
		
		m_VA->Bind();
		m_Shader->Bind();
		m_TexCube->Bind(0);
		m_Shader->SetUniform("u_Skybox", 0);

		soso::Renderer::SubmitSkybox(m_Shader, m_VA);
		m_VA->Unbind();
		
	}

};