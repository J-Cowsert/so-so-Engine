#include "sspch.h"
#include "VertexBuffer.h"
#include "Renderer.h"
#include "so-so/RenderAPI/OpenGL/OpenGLVertexBuffer.h"

namespace soso {

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size) {

		switch (Renderer::GetAPI()) 
		{
			case RendererAPI::API::None:
				SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLVertexBuffer>(size);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(void* verticies, uint32_t size) {
		
		switch (Renderer::GetAPI()) 
		{
			case RendererAPI::API::None:
				SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLVertexBuffer>(verticies, size);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}
}