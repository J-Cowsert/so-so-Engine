#include <soso.h>

#include "imgui/imgui.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/vec4.hpp"

class ExampleLayer : public soso::Layer {
public:

	ExampleLayer()
		:Layer("Example") 
	{
		m_Color = glm::vec4(0.1f, 1.0f, 0.5f, 1.0);
	}

	void OnUpdate() override {
		
	}
	
	void OnImGuiRender() override {
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void OnEvent(soso::Event& event) override {
		//SS_TRACE("{0}", event);
	}
	
private:
	glm::vec4 m_Color;
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