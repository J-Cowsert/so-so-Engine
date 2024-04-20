#pragma once

#ifdef SS_PLATFORM_WINDOWS

extern  soso::Application* soso::CreateApplication();

int main(int argc, char** argv) {

	soso::Log::Init();
	SS_INFO("Log::Init in EntryPoint.h\n");

	auto app = soso::CreateApplication();
	app->Run();
	delete app;
}
#endif // SS_PLATFORM_WINDOWS