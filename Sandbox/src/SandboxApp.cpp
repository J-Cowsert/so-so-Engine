#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
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