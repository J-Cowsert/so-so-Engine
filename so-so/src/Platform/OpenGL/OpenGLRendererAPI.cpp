#include "sspch.h"
#include "OpenGLRendererAPI.h"

#include "glad/glad.h"

namespace soso {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam) {
		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:         SS_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       SS_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          SS_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: SS_CORE_TRACE(message); return;
		}

		SS_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init() {

	#ifdef SS_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	#endif

		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);


		//glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);
	}


	void OpenGLRendererAPI::EnableDepthTest() {
		//glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::EnableCulling() {
		//glEnable(GL_CULL_FACE);

		//temp
		//glCullFace(GL_BACK);
		//glFrontFace(GL_CCW);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) {
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}