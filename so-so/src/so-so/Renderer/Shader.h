#pragma once

#include "so-so/Renderer/UniformBuffer.h"

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

namespace soso {

	enum class ShaderStage {
		None = 0,
		Vertex,
		Geometry, // TODO
		Fragment,
		Compute
	};

	enum class ShaderUniformType {
		None = 0, 
		Bool, Int, Float, Vec2, Vec3, Vec4, Mat3, Mat4, UInt, IVec2, IVec3, IVec4
	};

	class ShaderUniform {
	public:
		ShaderUniform() = default;
		ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset)
			: m_Name(name), m_Type(type), m_Size(size), m_Offset(offset) 
		{
		}

		const std::string& GetName() const { return m_Name; }
		ShaderUniformType GetType() const { return m_Type; }
		uint32_t GetSize() const { return m_Size; }
		uint32_t GetOffset() const { return m_Offset; }

	private:
		std::string m_Name;
		ShaderUniformType m_Type = ShaderUniformType::None;
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;
	};

	// TODO: Move some of this to RenderAPI backend as it is api specific. Vulkan for example needs more data stored like set

	struct ShaderUniformBufferInfo { 

		std::string Name;
		uint32_t Size;
		uint32_t BindingPoint;
		uint32_t RendererID;
		std::unordered_map<std::string, ShaderUniform> Uniforms;
	};

	// This class defines how a specific resource (i.e. Texture sampler) is bound to a specific location in a shader program. 
	class ShaderResourceInfo {
	public:
		ShaderResourceInfo() = default;
		ShaderResourceInfo(const std::string& name, uint32_t bindingPoint) 
			: m_Name(name), m_BindingPoint(bindingPoint)
		{
		}

		const std::string& GetName() const { return m_Name; }
		uint32_t GetBindingPoint() const { return m_BindingPoint; }

	private:
		std::string m_Name;
		uint32_t m_BindingPoint = 0;
	};

	class Shader {
	public:
		static std::shared_ptr<Shader> Create(const std::string& filepath);
		static std::shared_ptr<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~Shader() = default;

		virtual void Bind() const = 0;

		virtual void Reload() = 0;

		virtual const std::string& GetName() const = 0;
		virtual const std::unordered_map<std::string, ShaderUniformBufferInfo>& GetShaderBuffers() const = 0;
		virtual const std::unordered_map<std::string, ShaderResourceInfo>& GetResources() const = 0;
		virtual std::unordered_map<ShaderStage, std::string> GetSources() const = 0;
		virtual const std::string& GetFilepath() const = 0;

		virtual std::shared_ptr<UniformBuffer> GetUniformBuffer(uint32_t bindingPoint) const = 0;

		virtual void SetUniform(const std::string& name, float value) = 0;
		virtual void SetUniform(const std::string& name, int value) = 0;
		virtual void SetUniform(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniform(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniform(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetUniform(const std::string& name, const glm::mat3& matrix) = 0;
		virtual void SetUniform(const std::string& name, const glm::mat4& matrix) = 0;
	};
}
