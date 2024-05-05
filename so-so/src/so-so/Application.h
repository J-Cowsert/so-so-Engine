#pragma once

#include "core.h"
#include "Window.h"
#include "so-so/LayerStack.h"
#include "so-so/Events/Event.h"
#include "so-so/Events/ApplicationEvent.h"

#include "so-so/ImGui/ImGuiLayer.h"

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

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication(); 

}