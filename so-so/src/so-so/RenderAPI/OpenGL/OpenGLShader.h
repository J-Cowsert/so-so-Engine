#pragma once

#include "so-so/Renderer/Shader.h"

#include "unordered_map"
#include <utility> 
#include <functional>
#include <expected>

#include "glad/glad.h"

namespace soso {

	class UniformBuffer;

	class OpenGLShader : public Shader {
	public:
		struct ReflectionData {
			std::unordered_map<std::string, ShaderUniformBufferInfo> ShaderUniformBufferInfo;   // Name, ShaderUniformBufferInfo
			std::unordered_map<std::string, ShaderResourceInfo> ShaderResourceInfo; // Name, ShaderResourceInfo
		};
		
	public:
		OpenGLShader(const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
		virtual ~OpenGLShader();

		void Bind() const override;
		
		void Reload() override;

		const std::string& GetName() const override { return m_Name; }
		const std::unordered_map<std::string, ShaderUniformBufferInfo>& GetShaderBuffers() const override { return m_ReflectionData.ShaderUniformBufferInfo; }
		const std::unordered_map<std::string, ShaderResourceInfo>& GetResources() const override { return m_ReflectionData.ShaderResourceInfo; }
		std::unordered_map<ShaderStage, std::string> GetSources() const override { return m_ShaderStageSources; }
		const std::string& GetFilepath() const override { return m_Filepath; }


		std::shared_ptr<UniformBuffer> GetUniformBuffer(uint32_t bindingPoint) const override;

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
		
		std::expected<void, std::string> Compile(std::unordered_map<ShaderStage, std::string> shaderSources, bool forceReload = false);
		void UploadToGPU();

		void Reflect(ShaderStage stage, const std::vector<uint32_t>& data);

		void CreateUniformBufferObjects();

	private:
		uint32_t m_RendererID;
		std::string m_Name, m_Filepath;

		std::unordered_map<ShaderStage, std::string> m_ShaderStageSources;
		std::unordered_map<ShaderStage, std::vector<uint32_t>> m_OpenGLSpirv;

		ReflectionData m_ReflectionData;
 
		// We want to store all of the uniform buffers for every shader in one location. 
		// If only the binding point were used as the key you would run in to conflicts, as seperate shaders might have UBOs using
		// the same binding.
		// If both binding point and RendererID were used, you might run into problems when the shader is recompiled and the rendererID changes. 

		// These will need to be reloaded when a shader is reloaded or recompiled as the Uniform buffer might change.
		// If a shader instance is destroyed, it's pointer will become invalid. This should be okay since the only way you can get a shader's uniform buffer is using that shader.
		// 
		// Currently only stores Material related uniform buffer objects

		inline static std::map<std::pair<const OpenGLShader*, uint32_t>, std::shared_ptr<UniformBuffer>> s_UniformBuffers; // key  = pair<OpenGLShader*, bindingPoint>
	};
}
