#include "sspch.h"

#include "IndexBuffer.h"

#include "Renderer.h"

#include "so-so/RenderAPI/OpenGL/OpenGLIndexBuffer.h"

namespace soso {

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t size) {

		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(size);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(void* indices, uint32_t size) {

		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(indices, size);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}
}