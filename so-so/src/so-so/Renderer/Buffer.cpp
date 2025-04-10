#include "sspch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "so-so/RendererAPI/OpenGL/OpenGLBuffer.h"

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

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(void* indices, uint32_t count) {

		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLIndexBuffer>(indices, count);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}
}