
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#pragma warning(disable: 4996)

#include "so-so/Core/EntryPoint.h"

#include "TestLayer.h"
#include "EditorLayer.h"


class Sandbox : public soso::Application {
public:
	Sandbox() {

		PushLayer<EditorLayer>(std::make_unique<TestLayer>());
	}

	~Sandbox() {}
};

soso::Application* soso::CreateApplication() {
	return new Sandbox();
}