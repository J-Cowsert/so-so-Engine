#include "sspch.h"
#include "Material.h"

#include "so-so/Renderer/Renderer.h"

#include "so-so/RenderAPI/OpenGL/OpenGLMaterial.h"

namespace soso {

	std::shared_ptr<Material> Material::Create(const std::shared_ptr<Shader>& shader, const std::string& name) {
	
		switch (Renderer::GetAPI()) {
	
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;
	
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLMaterial>(shader, name);
		}
	
		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}
}