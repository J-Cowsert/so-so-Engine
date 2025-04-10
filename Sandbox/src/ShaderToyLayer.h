#pragma once

#include "soso.h"
#include "so-so/Renderer/ShaderLibrary.h"

#include "imgui.h"

class ShaderToy : public soso::Layer {
public:
	ShaderToy() 
		:Layer("ShaderToy") 
	{
		OnAttach();
	}

	~ShaderToy() {}

	void OnAttach() override {

		SS_INFO("ShaderToy Layer Attached");

		float verts[] = {
           -1.0f, -1.0f, 0.0f, 
	        1.0f, -1.0f, 0.0f, 
	        1.0f,  1.0f, 0.0f, 
		   -1.0f,  1.0f, 0.0f
		};
		uint32_t indices[] = { 0, 2, 1, 2, 0, 3 };

		auto vb = soso::VertexBuffer::Create(verts, sizeof(verts));
		auto ib = soso::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

		vb->SetLayout({
			{soso::ShaderDataType::Float3, "a_Position"}
		});

		m_VertexArray = soso::VertexArray::Create();
		m_VertexArray->Bind();

		m_VertexArray->AddVertexBuffer(vb);
		m_VertexArray->SetIndexBuffer(ib);


		{
			m_ShaderLibrary.Load("Canvas", "assets/shaders/Canvas.glsl");
		}
	}

	void OnUpdate(soso::Timestep ts) override {
		
		soso::RenderCommand::SetClearColor({0, 0, 0, 1});
		soso::RenderCommand::Clear();

		m_iTime += ts.GetSeconds();

		{
			auto shader = m_ShaderLibrary.Get("Canvas");
			shader->Bind();
			shader->SetFloat3("iResolution", glm::vec3(m_iResolution, m_iResolution.x / m_iResolution.y));
			shader->SetFloat("iTime", m_iTime);
		}

		soso::RenderCommand::DrawIndexed(m_VertexArray);
	}

	void OnEvent(soso::Event& event) override {
		soso::EventDispatcher dispatcher(event);
		//dispatcher.Dispatch<soso::WindowResizeEvent>(SS_BIND_EVENT_FN(ShaderToy::OnWindowResize));
	}
	
	void OnImGuiRender() override {

		ImGui::Begin("Stats");
		{
			ImGui::Text("Resolution: %0.0f x %0.0f", m_iResolution.x, m_iResolution.y);
			ImGui::Text("Time: %f", m_iTime);
		}
		ImGui::End();


		ImGui::Begin("Shader Controls");
		{
			if (ImGui::Button("Recompile Shader")) {
				RecompileShader();
			}


			ImGui::Separator();


			if (ImGui::Button("Reset Time")) {
				ResetTime();
			}
			
			ImGui::SliderFloat("Set Time", &m_iTime, -10, m_iTime);
			
			if (ImGui::Button("iTime * 10")) {
				m_iTime *= 10;
			}

			if (ImGui::Button("iTime^2")) {
				m_iTime *= m_iTime;
			}
		}
		ImGui::End();


	}

	void SetResolution(const glm::vec2& resolution) {
		m_iResolution = resolution;
	}

	bool OnWindowResize(soso::WindowResizeEvent& event) {
		m_iResolution.x = event.GetWidth();
		m_iResolution.y = event.GetHeight();

		return true;
	}

	void RecompileShader() {
		m_ShaderLibrary.Load("Canvas", "assets/shaders/Canvas.glsl");
	}

	void ResetTime() { m_iTime = 0; }
	void SetTime(float time) { m_iTime = time; }
	const float& GetTime() const { return m_iTime; }

private:

	soso::ShaderLibrary m_ShaderLibrary;

	std::shared_ptr<soso::VertexArray> m_VertexArray;

	glm::vec2 m_iResolution = { soso::Application::Get().GetWindow().GetWidth(), soso::Application::Get().GetWindow().GetHeight() };
	float m_iTime = 0;
};