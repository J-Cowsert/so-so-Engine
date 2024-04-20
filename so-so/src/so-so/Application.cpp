#include "sspch.h"
#include "Application.h"

#include "so-so/Events/ApplicationEvent.h"
#include "so-so/Log.h"

namespace soso {

	Application::Application() {
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application() {

	}

	void Application::Run() {

		
		while (m_Running) {
			m_Window->OnUpdate();
		}
	}

}