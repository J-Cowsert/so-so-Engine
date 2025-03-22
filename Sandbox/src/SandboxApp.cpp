
#include "TestLayer.h"
#include "EditorLayer.h"
#include "ShaderToyLayer.h"


class Sandbox : public soso::Application {
public:
	Sandbox() {
		PushLayer(new TestLayer());
	}

	~Sandbox() {}
};

soso::Application* soso::CreateApplication() {
	return new Sandbox();
}