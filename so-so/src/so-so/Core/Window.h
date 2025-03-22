#pragma once

#include "sspch.h"

#include "so-so/Core/core.h"
#include "so-so/Events/Event.h"

#include "so-so/Renderer/GraphicsContext.h"
#include <GLFW/glfw3.h>


namespace soso {

	// TODO: Implement Fullscreen / Decorated 
	struct WindowConfig {
		std::string Title = "so-so";
		uint32_t Width = 1280;
		uint32_t Height = 1024;
		bool Fullscreen = false;
		bool Decorated = true;
		bool VSync = true;
	};

	class Window {
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

	public:
		Window(const WindowConfig& config);
		virtual ~Window();

		virtual void OnUpdate();

		virtual uint32_t GetWidth() const { return m_Data.Width; }
		virtual uint32_t GetHeight() const { return m_Data.Height; }

		virtual void SetEventCallback(const EventCallbackFunction& callback) { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled);
		virtual bool IsVSync() const;

		virtual void* GetNativeWindow() const { return m_Window; }

		static Window* Create(const WindowConfig& config = WindowConfig());

	private:
		virtual void Init(const WindowConfig& config);
		virtual void Shutdown();

	private:
		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData {

			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunction EventCallback;
		};

		WindowData m_Data;
	};
}