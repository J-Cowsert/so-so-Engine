#pragma once

#include "core.h"
#include "so-so/Events/Event.h"
#include "so-so/Events/ApplicationEvent.h"

#include "Window.h"

namespace soso {

	class SOSO_API Application
	{

	public:
		Application();
		virtual ~Application();
		 
		void Run();

		void OnEvent(Event& e);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in CLIENT
	Application* CreateApplication(); 

}