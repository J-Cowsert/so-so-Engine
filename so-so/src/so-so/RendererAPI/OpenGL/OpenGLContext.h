#pragma once
#include "so-so/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace soso {

	class OpenGLContext : public GraphicsContext {
	public: 
		OpenGLContext(GLFWwindow* contextWindow);

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_ContextWindow;
	};
}