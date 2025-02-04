#pragma once

#include "core.h"
#include "Timestep.h"
#include "Window.h"
#include "LayerStack.h"
#include "../Events/Event.h"
#include "../Events/ApplicationEvent.h"

#include "so-so/ImGui/ImGuiLayer.h"

#include "../Renderer/Shader.h"
#include "../Renderer/Buffer.h"
#include "../Renderer/VertexArray.h"

namespace soso {

	class SOSO_API Application
	{
	public:
		Application();
		virtual ~Application() = default;
		 
		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication(); 

}