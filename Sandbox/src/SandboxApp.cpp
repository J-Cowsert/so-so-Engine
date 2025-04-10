
#include "TestLayer.h"
#include "EditorLayer.h"


class Sandbox : public soso::Application {
public:
	Sandbox() {

		auto editor = new EditorLayer();

		auto runtime = std::make_unique<TestLayer>();
		editor->SetRuntimeLayer(std::move(runtime));

		PushLayer(editor);
	}

	~Sandbox() {}
};

soso::Application* soso::CreateApplication() {
	return new Sandbox();
}