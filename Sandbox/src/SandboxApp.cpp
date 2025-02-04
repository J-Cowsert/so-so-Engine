#include <soso.h>
#include <so-so/Renderer/Renderer.h>
#include <so-so/Renderer/SceneCamera.h>
#include <so-so/Renderer/Model.h>
#include "imgui/imgui.h"
#include <filesystem>
#include "glm/glm.hpp"
#include "glm/glm/ext.hpp"

class ExampleLayer : public soso::Layer {
public:

	ExampleLayer()
		:Layer("Example") 
	{
		SS_CORE_INFO("Current working directory: {0}", std::filesystem::current_path().string());
		soso::Renderer::Init();


		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Normal;
			layout(location = 2) in vec2 a_TexCoord;

			uniform mat4 u_Model;
			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec3 v_Normal;
			out vec2 v_TexCoord;

			void main()
			{
				v_Position = a_Position;
				v_Normal = a_Normal;
				v_TexCoord = a_TexCoord;
				gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);	
			}

		)";
		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Normal;


			void main()
			{
				color = vec4(normalize(v_Normal) * 0.5 + 0.5, 1.0);
			}
		)";

		m_Shader = soso::Shader::Create(vertexSrc, fragmentSrc);

		m_Camera = std::make_unique<soso::SceneCamera>(glm::radians(100.0f), (float)soso::Application::Get().GetWindow().GetWidth() / soso::Application::Get().GetWindow().GetHeight(), 0.1f, 10000.0f);
		m_Model = std::make_shared<soso::Model>("assets/models/teapot.obj");
	}

	void OnUpdate(soso::Timestep ts) override {

		soso::Renderer::BeginScene();
		{
			m_Camera->OnUpdate(ts);
			m_Rotation += m_RotationSpeed;

			glm::mat4 transform = glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0.5, 0.1));
			transform = glm::scale(glm::mat4(1.0f), glm::vec3(m_TransformScale));
			glm::mat4 viewProjection = m_Camera->GetViewProjection();

			m_Shader->Bind();
			m_Shader->SetMat4("u_Model", transform);
			m_Shader->SetMat4("u_ViewProjection", viewProjection);

			m_Model->Draw();
		}
		soso::Renderer::EndScene();
	}
	
	void OnImGuiRender() override {
		static bool show = false;
		
		{
			ImGui::Begin("Debug");

			if (ImGui::CollapsingHeader("Scene Parameters")) {
				
				ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 0.0f, 0.09f);
				ImGui::SliderFloat("Transform Scale", &m_TransformScale, 0.01f, 1.0f);
			}
			
			ImGui::End();
		}
	}

	void OnEvent(soso::Event& event) override {
		//soso::EventDispatcher dispatcher(event);
	}
	
private:

	float m_Rotation = 0.0f;
	float m_RotationSpeed = 0.0f;

	float m_TransformScale = 1.0f;

	std::shared_ptr<soso::Shader> m_Shader;
	std::shared_ptr<soso::VertexArray> m_VertexArray;
	std::shared_ptr<soso::VertexBuffer> m_VertexBuffer;
	std::shared_ptr<soso::IndexBuffer> m_IndexBuffer;

	std::unique_ptr<soso::SceneCamera> m_Camera;
	std::shared_ptr<soso::Model> m_Model;
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