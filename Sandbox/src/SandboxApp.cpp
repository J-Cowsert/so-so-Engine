#include <soso.h>

#include "imgui/imgui.h"

class ExampleLayer : public soso::Layer {
public:

	ExampleLayer()
		:Layer("Example") 
	{
	}

	void OnUpdate() override {
		
	}
	
	void OnImGuiRender() override {
       
	}

	void OnEvent(soso::Event& event) override {
		//SS_TRACE("{0}", event);
	}
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