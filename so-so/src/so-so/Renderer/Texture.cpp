#include "sspch.h"
#include "Texture.h"

#include "Renderer.h"
#include "so-so/RenderAPI/OpenGL/OpenGLTexture.h"


namespace soso {


    std::shared_ptr<Texture2D> Texture2D::Create(const TextureConfig& config, ByteBuffer data) {

		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:    SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(config, data);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
    }

	std::shared_ptr<Texture2D> Texture2D::Create(const TextureConfig& config, const std::filesystem::path& filepath)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:    SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTexture2D>(config, filepath);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	std::shared_ptr<TextureCube> TextureCube::Create(const TextureConfig& config, const std::array<ByteBuffer, 6>& data) {

		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:    SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLTextureCube>(config, data);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}