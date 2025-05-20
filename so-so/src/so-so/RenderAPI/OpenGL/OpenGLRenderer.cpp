#include "sspch.h"
#include "OpenGLRenderer.h"

#include "glad/glad.h"

namespace soso {

	static void OpenGLDebugCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam) 
	{
		switch (severity) 
		{
		case GL_DEBUG_SEVERITY_HIGH:         SS_CORE_CRITICAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       SS_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          SS_CORE_WARN(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: SS_CORE_TRACE(message); return;
		}

		SS_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRenderer::Init() {


	#ifdef SS_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLDebugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	#endif

		//glEnable(GL_MULTISAMPLE);

		//glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_LINE_SMOOTH);
		//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		//glEnable(GL_FRAMEBUFFER_SRGB);

		SetClearColor(1, 0, 1, 1);

		GLenum error = glGetError();
		while (error != GL_NO_ERROR) {
			SS_CORE_ERROR("OpenGL Error {0}", error);
			error = glGetError();
		}

		glUseProgram(0);
	}


	namespace Utils {

		static GLenum ConvertToGLDepthFunc(RendererAPI::DepthFunction func) {

			switch (func) {
				case RendererAPI::DepthFunction::LESS: return GL_LESS;
				case RendererAPI::DepthFunction::LEQUAL: return GL_LEQUAL;
				case RendererAPI::DepthFunction::GREATER: return GL_GREATER;
				case RendererAPI::DepthFunction::GEQUAL: return GL_GEQUAL;
				case RendererAPI::DepthFunction::EQUAL: return GL_EQUAL;
				case RendererAPI::DepthFunction::ALWAYS: return GL_ALWAYS;
				case RendererAPI::DepthFunction::NEVER: return GL_NEVER;
				default: return GL_LESS;
			}
		}
	}

	void OpenGLRenderer::SetDepthFunction(DepthFunction func) {
		glDepthFunc(Utils::ConvertToGLDepthFunc(func));
	}

	void OpenGLRenderer::EnableDepthTest() {
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRenderer::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRenderer::SetClearColor(float r, float g, float b, float a) {
		glClearColor(r, g, b, a);
	}

	void OpenGLRenderer::Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRenderer::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, const uint32_t indexCount) {

		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, (indexCount) ? indexCount : vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRenderer::DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) {
		
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRenderer::SetLineWidth(float width) {
		glLineWidth(width);
	}
}