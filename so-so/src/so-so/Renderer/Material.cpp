#include "sspch.h"
#include "Material.h"


namespace soso {

	Material::Material(const std::shared_ptr<Shader>& shader) 
		:m_Shader(shader)
	{
	}

	void Material::Bind() {

		m_Shader->Bind();
		m_Shader->SetFloat("u_Material.Shininess", m_Shininess);

		m_Shader->SetInt("u_Material.Diffuse", 0);
		m_Shader->SetInt("u_Material.Specular", 1);

		if (m_DiffuseMap)
			m_DiffuseMap->Bind(0);

		if (m_SpecularMap)
			m_SpecularMap->Bind(1);
	}

	std::shared_ptr<Material> Material::Create(const std::shared_ptr<Shader>& shader) {
		return std::make_shared<Material>(shader);
	}
}