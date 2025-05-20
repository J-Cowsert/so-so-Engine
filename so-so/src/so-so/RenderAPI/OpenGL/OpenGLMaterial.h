#pragma once

#include "so-so/Renderer/Material.h"
#include "so-so/Renderer/UniformBuffer.h"

namespace soso {
	
	class OpenGLMaterial : public Material {
	public:
		OpenGLMaterial(const std::shared_ptr<Shader>& shader, const std::string& name);
		OpenGLMaterial(std::shared_ptr<Material> material, const std::string& name);
		virtual ~OpenGLMaterial() override;
	
		SS_FORCE_INLINE void Bind() override; // Material shouldn't be responsible for binding before the draw call
	
		std::shared_ptr<Shader> GetShader() const override { return m_Shader; }
		uint32_t GetFlags() const override { return m_MaterialFlags; }

		void EnableFlag(MaterialFlag flag) { m_MaterialFlags |= (uint32_t)flag; }
		void DisableFlag(MaterialFlag flag) { m_MaterialFlags &= ~(uint32_t)flag; }

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
			SS_CORE_ASSERT(sizeof(T) == info->GetSize(), "Size mismatch");
			
			if (!info) {
				SS_CORE_WARN("Name: {0}", name);
				return;
			}

			ByteBuffer& buffer = m_UniformByteBuffer;
			buffer.Write(&value, info->GetSize(), info->GetOffset());

			// Set dirty flag true so that the uniform is reset.
			m_IsDirty = true; // TODO: remove
		}

		// TODO: Getters

	private:
		const ShaderUniform* FindUniformInfo(const std::string& name);
		const ShaderResourceInfo* FindResourceInfo(const std::string& name);
		void AllocateByteBufferMemory();
	
	private:
		std::shared_ptr<Shader> m_Shader;
		std::string m_Name;
		uint32_t m_MaterialFlags = (uint32_t)MaterialFlag::None;
		ByteBuffer m_UniformByteBuffer;
		
		std::map<uint32_t, std::shared_ptr<Texture2D>> m_Textures;

		std::shared_ptr<UniformBuffer> m_UniformBuffer; // sus - should I cache this?
		bool m_IsDirty = false; // TODO: remove. Not currently doing anything useful
	};
}