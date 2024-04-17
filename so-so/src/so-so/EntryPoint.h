#pragma once

#ifdef SS_PLATFORM_WINDOWS

extern  soso::Application* soso::CreateApplication();

int main(int argc, char** argv) {
	auto app = soso::CreateApplication();
	app->Run();

	delete app;
}
#endif // SS_PLATFORM_WINDOWS

