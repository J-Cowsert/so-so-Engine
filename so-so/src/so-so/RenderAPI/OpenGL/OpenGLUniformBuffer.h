#pragma once
#include "so-so/Renderer/UniformBuffer.h"

namespace soso {

	class OpenGLUniformBuffer : public UniformBuffer {
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t bindingPoint);
		virtual ~OpenGLUniformBuffer();

		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
	};

}