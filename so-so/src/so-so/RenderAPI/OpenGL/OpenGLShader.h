#pragma once

#include "so-so/Renderer/Shader.h"
#include "unordered_map"
#include "glad/glad.h"

namespace soso {

	enum class ShaderStage {
		None = 0,
		Vertex,
		Fragment,
		//TODO: implement
		Compute
	};

	class OpenGLShader : public Shader {
	public:
		struct ReflectionData {
			std::unordered_map<std::string, ShaderBuffer> ShaderBuffers;   // Name, ShaderBuffer
			std::unordered_map<std::string, ShaderResourceInfo> Resources; // Name, ShaderResourceInfo
		};
		
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;
		
		void Reload() const override;

		const std::string& GetName() const override { return m_Name; }
		const std::unordered_map<std::string, ShaderBuffer>& GetShaderBuffers() const override { return m_ReflectionData.ShaderBuffers; }
		const std::unordered_map<std::string, ShaderResourceInfo>& GetResources() const override { return m_ReflectionData.Resources; }

		void SetUniform(const std::string& name, int value) override;
		void SetUniform(const std::string& name, float value) override;
		void SetUniform(const std::string& name, const glm::vec2& value) override;
		void SetUniform(const std::string& name, const glm::vec3& value) override;
		void SetUniform(const std::string& name, const glm::vec4& value) override;
		void SetUniform(const std::string& name, const glm::mat3& matrix) override;
		void SetUniform(const std::string& name, const glm::mat4& matrix) override;

	private:
		void UploadInt(const std::string& name, int value);
		void UploadFloat(const std::string& name, float value);
		void UploadVec2(const std::string& name, const glm::vec2& value);
		void UploadVec3(const std::string& name, const glm::vec3& value);
		void UploadVec4(const std::string& name, const glm::vec4& value);
		void UploadMat3(const std::string& name, const glm::mat3& value);
		void UploadMat4(const std::string& name, const glm::mat4& value);

	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<ShaderStage, std::string> PreProcess(const std::string& source);
		
		void Compile(std::unordered_map<ShaderStage, std::string> shaderSources, bool forceReload = false);
		void Upload();

		void Reflect(ShaderStage stage, const std::vector<uint32_t>& data);

	private:
		uint32_t m_RendererID;
		std::string m_Name, m_Filepath;

		std::unordered_map<ShaderStage, std::string> m_ShaderStages; // Rename to shaderStageSources
		std::unordered_map<ShaderStage, std::vector<uint32_t>> m_OpenGLSpirv;
		ReflectionData m_ReflectionData;
	};
}