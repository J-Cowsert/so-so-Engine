#include "sspch.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "so-so/RendererAPI/OpenGL/OpenGLFrameBuffer.h"


namespace soso {

	std::shared_ptr<FrameBuffer> FrameBuffer::Create(const FrameBufferConfig& config) {

		switch (Renderer::GetAPI()) {

			case RendererAPI::API::None:
				SS_CORE_ASSERT(false, "RendererAPI is set to None"); return nullptr;

			case RendererAPI::API::OpenGL:
				return std::make_shared<OpenGLFrameBuffer>(config);
		}

		SS_CORE_ASSERT(false, "RendererAPI is undefined");
		return nullptr;
	}
}