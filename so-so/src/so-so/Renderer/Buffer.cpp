#include "sspch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace soso {

	VertexBuffer* VertexBuffer::Create(float* verticies, uint32_t size) {
		
		switch (Renderer::GetAPI()) 
		{
		case RendererAPI::API::None:
				SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

		case RendererAPI::API::OpenGL:
				return new OpenGLVertexBuffer(verticies, size);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count) {

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
				SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

		case RendererAPI::API::OpenGL:
				return new OpenGLIndexBuffer(indices, count);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}
}