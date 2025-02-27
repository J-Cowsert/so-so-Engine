#include "sspch.h"
#include "Texture.h"

#include "so-so/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"


namespace soso {


    std::shared_ptr<Texture2D> Texture2D::Create(const TextureProvision& provision, Buffer data) {

		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:    SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(provision, data);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
    }

	std::shared_ptr<TextureCube> TextureCube::Create(const TextureProvision& provision, const std::array<Buffer, 6>& data) {

		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:    SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTextureCube>(provision, data);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}