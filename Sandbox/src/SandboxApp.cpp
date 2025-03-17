#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS

#include "TestLayer.h"
#include "EditorLayer.h"
#include "ShaderToyLayer.h"


class Sandbox : public soso::Application {
public:
	Sandbox() {
		PushLayer(new EditorLayer());
	}

	~Sandbox() {}
};

soso::Application* soso::CreateApplication() {
	return new Sandbox();
}