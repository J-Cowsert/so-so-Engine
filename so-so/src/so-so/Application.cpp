#include "sspch.h"
#include "Application.h"

#include "so-so/Events/ApplicationEvent.h"
#include "so-so/Log.h"

namespace soso {

	Application::Application() {

	}

	Application::~Application() {

	}

	void Application::Run() {

		WindowResizeEvent e(1980, 1000);
		SS_TRACE(e);

		while (true);
	}

}