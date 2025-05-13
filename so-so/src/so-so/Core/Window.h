#pragma once
#include "sspch.h"

#include "so-so/Core/Core.h"
#include "so-so/Events/Event.h"
#include "so-so/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>


namespace soso {

	struct WindowConfig {

		std::string Title = "so-so";
		uint32_t Width = 1920, Height = 1080;
		bool Fullscreen = false;
		bool VSync = true;
	};

	class Window {
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

	public:
		Window(const WindowConfig& config);
		~Window();

		void OnUpdate();

		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }

		void SetEventCallback(const EventCallbackFunction& callback) { m_Data.EventCallback = callback; }
		
		void Maximize();

		void SetFullscreen(bool enabled);
		void SetVSync(bool enabled);
		bool IsFullscreen() const;
		bool IsVSync() const;

		void* GetNativeWindow() const { return m_Window; }

		static Window* Create(const WindowConfig& config = WindowConfig());

	private:
		void Init(const WindowConfig& config);
		void Shutdown();

	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData {

			std::string Title;
			uint32_t Width, Height;
			bool Fullscreen;
			bool VSync;

			// Keep track of position and size when switching from windowed to fullscreen. 
			uint32_t WindowedX, WindowedY, WindowedWidth, WindowedHeight;

			EventCallbackFunction EventCallback;
		};

		WindowData m_Data;
	};
}