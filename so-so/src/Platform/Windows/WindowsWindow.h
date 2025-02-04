#pragma once

#include "so-so/Core/Window.h"
#include "so-so/Renderer/GraphicsContext.h"

#include "GLFW/glfw3.h"


namespace soso {

	class WindowsWindow : public Window {

	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		unsigned int GetWidth()  const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		// Attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline void* GetNativeWindow() const override { return m_Window; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

		GraphicsContext* m_Context;

	private:
		GLFWwindow* m_Window;

		struct WindowData {

			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}