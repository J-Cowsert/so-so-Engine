#include <soso.h>

class Sandbox : public soso::Application {
public:
	Sandbox() {

	}

	~Sandbox() {

	}
};

soso::Application* soso::CreateApplication() {
	return new Sandbox();
}