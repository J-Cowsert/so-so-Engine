#include "sspch.h"
#include "Shader.h"

#include "Renderer.h"
#include "so-so/RendererAPI/OpenGL/OpenGLShader.h"

namespace soso {

	std::shared_ptr<Shader> Shader::Create(const std::string& filepath) {

		switch (Renderer::GetAPI()) 
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(filepath);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}

	std::shared_ptr<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}
}