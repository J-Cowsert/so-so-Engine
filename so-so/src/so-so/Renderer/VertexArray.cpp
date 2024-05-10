#include "sspch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace soso {


	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetRendererAPI())
		{
		case RendererAPI::None:
			SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

		case RendererAPI::OpenGL:
			return new OpenGLVertexArray();
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}
}