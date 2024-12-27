#pragma once

#include "so-so/Renderer/Shader.h"
#include "glm/glm.hpp"

namespace soso {

	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

	private:
		uint32_t m_RendererID;
	};

}