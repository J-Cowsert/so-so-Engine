#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "Timestep.h"
#include "so-so/Events/ApplicationEvent.h"
#include "so-so/ImGui/ImGuiLayer.h"


namespace soso {

	struct ApplicationConfig {
		std::string name = "so-so engine";
		std::string workingDirectory;
	};

	class Application
	{
	public:
		Application();
		virtual ~Application() = default;
		 
		void Run();
		void Close() { m_Running = false; }

		void OnEvent(Event& e);

		template<typename T, typename... Args>
		void PushLayer(Args&&... args) {
			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer");
			static_assert(std::is_constructible_v<T, Args...>, "Layer type T is not constructible with the given arguments");
			T* layer = new T(std::forward<Args>(args)...);
			m_LayerStack.PushLayer(layer);
			layer->OnAttach();
		}

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }
		const float GetFPS() const;

		static Application& Get() { return *s_Instance; }

		void SubmitToMainThread(const std::function<void()>& function);

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		void ExecuteMainThreadQueue();

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

		bool m_Running = true;
		bool m_Minimized = false;
		float m_LastFrameTime = 0.0f;
		float m_FPS = 0.0f;

		std::mutex m_MainThreadQueueMutex;
		std::vector<std::function<void()>> m_MainThreadQueue;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication(); 

}