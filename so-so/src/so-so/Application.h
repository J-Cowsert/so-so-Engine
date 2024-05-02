#pragma once

#include "core.h"
#include "Window.h"
#include "so-so/LayerStack.h"
#include "so-so/Events/Event.h"
#include "so-so/Events/ApplicationEvent.h"

namespace soso {

	class SOSO_API Application
	{

	public:
		Application();
		virtual ~Application();
		 
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	// To be defined in CLIENT
	Application* CreateApplication(); 

}