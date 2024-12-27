#include <soso.h>
#include <so-so/Renderer/Renderer.h>
#include "imgui/imgui.h"

class ExampleLayer : public soso::Layer {
public:

	ExampleLayer()
		:Layer("Example") 
	{
		m_VertexArray = soso::VertexArray::Create();

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f,		1.0f, 0.1f, 0.5f, 1.0f,
			 0.5f, -0.5f, 0.0f,		0.1f, 1.0f, 0.5f, 1.0f,
			 0.0f,  0.5f, 0.0f,		0.3f, 0.1f, 1.0f, 1.0f
		};

		m_VertexBuffer.reset(soso::VertexBuffer::Create(vertices, sizeof(vertices)));

		soso::BufferLayout layout = {

			{ soso::ShaderDataType::Float3, "a_Position" },
			{ soso::ShaderDataType::Float4, "a_Color" },
		};

		m_VertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		unsigned int indices[3] = { 0, 1, 2 };

		m_IndexBuffer.reset(soso::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;


			void main()
			{
				color = vec4(v_Position * 0.5 + 0.25, 1.0);
				color = v_Color;
			}
		)";

		m_Shader = soso::Shader::Create(vertexSrc, fragmentSrc);
	}

	void OnUpdate() override {
		soso::Renderer::BeginScene();

		m_Shader->Bind();
		soso::Renderer::Submit(m_VertexArray);

		soso::Renderer::EndScene();
	}
	
	void OnImGuiRender() override {
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void OnEvent(soso::Event& event) override {
		SS_TRACE("{0}", event);
	}
	
private:
	glm::vec4 m_Color;
	soso::Ref<soso::Shader> m_Shader;
	soso::Ref<soso::VertexArray> m_VertexArray;
	soso::Ref<soso::VertexBuffer> m_VertexBuffer;
	soso::Ref<soso::IndexBuffer> m_IndexBuffer;
};


class Sandbox : public soso::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};

soso::Application* soso::CreateApplication() {
	return new Sandbox();
}