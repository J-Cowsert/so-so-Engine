#pragma once

#include "so-so/Renderer/Shader.h"
#include "so-so/Renderer/Texture.h"
#include "so-so/Asset/Asset.h"

namespace soso {

	// Temporary for testing
	class Material {
	public:
		Material(const std::shared_ptr<Shader>& shader);
		~Material() = default;

		void Bind();

		std::shared_ptr<Shader> GetShader() const { return m_Shader; }

		void SetDiffuseMap(const std::shared_ptr<Texture2D>& diffuseMap) { m_DiffuseMap = diffuseMap; }
		void SetSpecularMap(const std::shared_ptr<Texture2D>& specularMap) { m_SpecularMap = specularMap; }
		void SetShininess(float shininess) { m_Shininess = shininess; }

	public:
		static std::shared_ptr<Material> Create(const std::shared_ptr<Shader>& shader);
	
	private:
		std::shared_ptr<Shader> m_Shader;

		std::shared_ptr<Texture2D> m_DiffuseMap;
		std::shared_ptr<Texture2D> m_SpecularMap;
		float m_Shininess;
	};
}