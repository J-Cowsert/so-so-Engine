#include "sspch.h"

#include "Window.h"
#include "so-so/Core/Log.h"
#include "so-so/Events/ApplicationEvent.h"
#include "so-so/Events/KeyEvent.h"
#include "so-so/Events/MouseEvent.h"

// TODO: Make this api agnostic. We shouldnt be using OpenGLContext directly
#include "so-so/RenderAPI/OpenGL/OpenGLContext.h"

namespace soso {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description) {
		SS_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowConfig& config) {
		return new Window(config);
	}

	Window::Window(const WindowConfig& config) {
		Init(config);
	}

	Window::~Window() {
		Shutdown();
	}

	void Window::Init(const WindowConfig& config) {

		m_Data.Title = config.Title;
		m_Data.Width = config.Width;
		m_Data.Height = config.Height;
		m_Data.Fullscreen = config.Fullscreen;
		m_Data.VSync = config.VSync;

		SS_CORE_INFO("Creating window {0} ({1}, {2})", m_Data.Title, m_Data.Width, m_Data.Height);

		if (!s_GLFWInitialized) {

			int success = glfwInit();
			SS_CORE_ASSERT(success, "Failed to initialize GLFW");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		if (m_Data.Fullscreen) {

			GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

			glfwWindowHint(GLFW_DECORATED, false);
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

			m_Window = glfwCreateWindow(mode->width, mode->height, m_Data.Title.c_str(), primaryMonitor, nullptr);
		}
		else {

			m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
		}

		m_Context = new OpenGLContext(m_Window); // TODO: Make api agnostic
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(m_Data.VSync);

		// GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}

			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int codepoint) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(codepoint);
			data.EventCallback(event);

		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}

			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void Window::Shutdown() {
		glfwDestroyWindow(m_Window);
	}

	void Window::OnUpdate() {
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void Window::SetVSync(bool enabled) {

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool Window::IsFullscreen() const {
		return m_Data.Fullscreen;
	}

	bool Window::IsVSync() const {
		return m_Data.VSync;
	}

	void Window::Maximize() {
		glfwMaximizeWindow(m_Window);
	}

	void Window::SetFullscreen(bool enabled) {
		
		if (enabled == m_Data.Fullscreen)
			return;

		if (enabled) {

			int tx, ty;
			glfwGetWindowPos(m_Window, &tx, &ty);
			m_Data.WindowedX = static_cast<uint32_t>(tx);
			m_Data.WindowedY = static_cast<uint32_t>(ty);

			int tw, th;
			glfwGetWindowSize(m_Window, &tw, &th);
			m_Data.WindowedWidth = static_cast<uint32_t>(tw);
			m_Data.WindowedHeight = static_cast<uint32_t>(th);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(
				m_Window,
				monitor,
				0, 0,                            // top‐left of the monitor
				mode->width,
				mode->height,
				mode->refreshRate
			);
		}
		else {
			// — Restoring windowed —
			glfwSetWindowMonitor(
				m_Window,
				nullptr,                        // NULL = windowed
				m_Data.WindowedX,
				m_Data.WindowedY,
				m_Data.WindowedWidth,
				m_Data.WindowedHeight,
				0                               // leave refresh rate untouched
			);
		}

		m_Data.Fullscreen = enabled;
	}
}