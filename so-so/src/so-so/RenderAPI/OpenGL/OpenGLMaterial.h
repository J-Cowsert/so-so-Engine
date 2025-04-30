#pragma once

#include "so-so/Renderer/Material.h"
#include "so-so/Renderer/UniformBuffer.h"

namespace soso {
	
	class OpenGLMaterial : public Material {
	public:
		OpenGLMaterial(const std::shared_ptr<Shader>& shader, const std::string& name);
		virtual ~OpenGLMaterial() override;
	
		void Bind(); 
	
		std::shared_ptr<Shader> GetShader() const override { return m_Shader; }
		uint8_t GetFlags() const override { return m_MaterialFlags; }

		void Set(const std::string& name, const float value) override;
		void Set(const std::string& name, const int value) override;
		void Set(const std::string& name, const glm::vec2 value) override;
		void Set(const std::string& name, const glm::vec3 value) override;
		void Set(const std::string& name, const glm::vec4 value) override;
		void Set(const std::string& name, const glm::mat2 value) override;
		void Set(const std::string& name, const glm::mat3 value) override;
		void Set(const std::string& name, const glm::mat4 value) override;

		void Set(const std::string& name, const std::shared_ptr<Texture2D>& texture) override;
	
		template <typename T>
		void Set(const std::string& name, const T& value) {

			const auto& info = FindUniformInfo(name);

			SS_CORE_ASSERT(info, "Could not find info");
			
			if (!info) {
				SS_CORE_WARN("Name: {0}", name);
				return;
			}

			auto& buffer = m_UniformByteBuffer;
			buffer.Write(&value, info->GetSize(), info->GetOffset());

			// Set dirty flag true so that the uniform is reset.
			m_IsDirty = true;
		}

	private:
		const ShaderUniform* FindUniformInfo(const std::string& name);
		const ShaderResourceInfo* FindResourceInfo(const std::string& name);
		void AllocateByteBufferMemory();
	
	private:
		std::shared_ptr<Shader> m_Shader;
		std::string m_Name;
		uint8_t m_MaterialFlags = (uint8_t)MaterialFlag::None;
		ByteBuffer m_UniformByteBuffer;
		
		std::map<uint32_t, std::shared_ptr<Texture2D>> m_Textures;

		std::shared_ptr<UniformBuffer> m_UniformBuffer; // Probably a better solution would be to have the shader create and hold all the ubos
		bool m_IsDirty = false; // Make sure to not reset data if not needed

	};
}