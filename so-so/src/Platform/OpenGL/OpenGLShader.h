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
		void SetFloat3(const std::string& name, const glm::vec3& value) override;
		void SetInt(const std::string& name, int value) override;
		void SetMat4(const std::string& name, const glm::mat4& value) override;

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformMat4(const std::string& name, const glm::mat4& value);

	private:
		uint32_t m_RendererID;
	};

}