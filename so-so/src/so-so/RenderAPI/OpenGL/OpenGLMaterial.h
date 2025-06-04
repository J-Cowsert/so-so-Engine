#pragma once

#include "so-so/Renderer/Material.h"
#include "so-so/Renderer/UniformBuffer.h"

namespace soso {
	
	class OpenGLMaterial : public Material {
	public:
		OpenGLMaterial(const std::shared_ptr<Shader>& shader, const std::string& name);
		OpenGLMaterial(std::shared_ptr<Material> material, const std::string& name);
		virtual ~OpenGLMaterial() override;
	
		SS_FORCE_INLINE void Bind() override;
	
		std::shared_ptr<Shader> GetShader() const override { return m_Shader; }

		uint32_t GetFlags() const override { return m_MaterialFlags; }
		bool IsFlagSet(MaterialFlag flag) const override { return m_MaterialFlags & (uint32_t)flag; }
		void EnableFlag(MaterialFlag flag) override { m_MaterialFlags |= (uint32_t)flag; }
		void DisableFlag(MaterialFlag flag) override { m_MaterialFlags &= ~(uint32_t)flag; }

		void Set(const std::string& name, const float value) override;
		void Set(const std::string& name, const int value) override;
		void Set(const std::string& name, const glm::vec2 value) override;
		void Set(const std::string& name, const glm::vec3 value) override;
		void Set(const std::string& name, const glm::vec4 value) override;
		void Set(const std::string& name, const glm::mat2 value) override;
		void Set(const std::string& name, const glm::mat3 value) override;
		void Set(const std::string& name, const glm::mat4 value) override;
		void Set(const std::string& name, const std::shared_ptr<Texture2D>& value) override;
		void Set(const std::string& name, const std::shared_ptr<TextureCube>& value) override;

	
		template <typename T>
		void Set(const std::string& name, const T& value) {

			const auto& info = FindUniformInfo(name);

			SS_CORE_ASSERT(info, "Could not find info");
			
			if (!info) {
				SS_CORE_WARN("Name: {0}", name);
				return;
			}
			SS_CORE_ASSERT(sizeof(T) == info->GetSize(), "Size mismatch");

			ByteBuffer& buffer = m_UniformByteBuffer;
			buffer.Write(&value, info->GetSize(), info->GetOffset());

			m_IsDirty = true; // TODO: The cached UB should only need to be reset if the shader is reloaded.
		}

		// TODO: Getters

		std::shared_ptr<TextureCube> GetTextureCube(const std::string& name) override;

	private:
		const ShaderUniform* FindUniformInfo(const std::string& name);
		const ShaderResourceInfo* FindResourceInfo(const std::string& name);
		void AllocateByteBufferMemory();
	
	private:
		std::string m_Name;
		std::shared_ptr<Shader> m_Shader;

		uint32_t m_MaterialFlags = (uint32_t)MaterialFlag::None;
		
		std::map<uint32_t, std::shared_ptr<Texture>> m_Texture2Ds; // <bindingPoint, texture2d>

		ByteBuffer m_UniformByteBuffer;
		std::shared_ptr<UniformBuffer> m_UniformBuffer; // Cached
		bool m_IsDirty = false;
	};
}