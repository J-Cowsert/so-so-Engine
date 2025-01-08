#pragma once

#include "so-so/Renderer/Shader.h"

namespace soso {

	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		void SetFloat(const std::string& name, float value) override;
		void SetMat4(const std::string& name, const glm::mat4& value) override;

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformMat4(const std::string& name, const glm::mat4& value);

	private:
		uint32_t m_RendererID;
	};

}