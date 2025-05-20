#include "sspch.h"
#include "ShaderLibrary.h"

namespace soso {

	void ShaderLibrary::Add(const std::string& name, const std::shared_ptr<Shader>& shader) {
		SS_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const std::shared_ptr<Shader>& shader) {
		auto& name = shader->GetName();
		Add(name, shader);
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& filepath) {
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath) {
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	std::shared_ptr<Shader> ShaderLibrary::Get(const std::string& name) {
		SS_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	std::vector<std::shared_ptr<Shader>> ShaderLibrary::GetAll() {

		std::vector<std::shared_ptr<Shader>> result;
		result.reserve(m_Shaders.size());

		for (const auto& [name, shader] : m_Shaders)
			result.push_back(shader);

		return result;
	}

	bool ShaderLibrary::Exists(const std::string& name) const {
		return m_Shaders.find(name) != m_Shaders.end();
	}

}