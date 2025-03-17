#pragma once

#include "so-so/Renderer/Shader.h"
#include "unordered_map"
#include "glad/glad.h"

namespace soso {

	enum class ShaderType {
		None = 0,
		Vertex,
		Fragment
	};

	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

		void SetFloat(const std::string& name, float value) override;
		void SetFloat3(const std::string& name, const glm::vec3& value) override;
		void SetInt(const std::string& name, int value) override;
		void SetMat4(const std::string& name, const glm::mat4& matrix) override;

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformMat4(const std::string& name, const glm::mat4& value);

		const std::string& GetName() const override { return m_Name; }

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source);
		void Compile(std::unordered_map<ShaderType, std::string> shaderSources);

	private:
		

		uint32_t m_RendererID;
		std::string m_Name;

		std::unordered_map<ShaderType, std::string> m_ShaderSources;
	};
}