#pragma once

#include "so-so/Renderer/Shader.h"
#include "so-so/Renderer/Texture.h"
#include "so-so/Asset/Asset.h"

namespace soso {

	enum class MaterialFlag {

		None = BIT(0),
		DepthTest = BIT(1),
		Blend = BIT(2),
		TwoSided = BIT(3)
	};

	class Material {
	public:
		static std::shared_ptr<Material> Create(const std::shared_ptr<Shader>& shader, const std::string& name = "");
		virtual ~Material() = default;
	
		virtual void Bind() = 0;
	
		virtual std::shared_ptr<Shader> GetShader() const = 0;
		virtual uint8_t GetFlags() const = 0;
	
		virtual void Set(const std::string& name, const float value) = 0;
		virtual void Set(const std::string& name, const int value) = 0;
		virtual void Set(const std::string& name, const glm::vec2 value) = 0;
		virtual void Set(const std::string& name, const glm::vec3 value) = 0;
		virtual void Set(const std::string& name, const glm::vec4 value) = 0;
		virtual void Set(const std::string& name, const glm::mat2 value) = 0;
		virtual void Set(const std::string& name, const glm::mat3 value) = 0;
		virtual void Set(const std::string& name, const glm::mat4 value) = 0;
	 
		virtual void Set(const std::string& name, const std::shared_ptr<Texture2D>& texture) = 0;
	};
}