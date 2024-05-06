#include "sspch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

soso::OpenGLContext::OpenGLContext(GLFWwindow* contextWindow)
	: m_ContextWindow(contextWindow) {

	SS_CORE_ASSERT(contextWindow, "windowContext is null");
}

void soso::OpenGLContext::Init() {

	glfwMakeContextCurrent(m_ContextWindow);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	SS_CORE_ASSERT(status, "Failed to initialize Glad.");

	SS_CORE_INFO("OpenGL Info:");
	//SS_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
	//SS_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
	//SS_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
}

void soso::OpenGLContext::SwapBuffers() {
	glfwSwapBuffers(m_ContextWindow);
}
