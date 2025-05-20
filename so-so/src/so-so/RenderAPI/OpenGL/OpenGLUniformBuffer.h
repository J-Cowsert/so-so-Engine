#pragma once
#include "so-so/Renderer/UniformBuffer.h"

namespace soso {

	class OpenGLUniformBuffer : public UniformBuffer {
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t bindingPoint);
		virtual ~OpenGLUniformBuffer();

		OpenGLUniformBuffer(const OpenGLUniformBuffer& other) = delete;
		OpenGLUniformBuffer& operator=(const OpenGLUniformBuffer&) = delete;

		void Bind() override;

		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

		uint32_t GetBindingPoint() { return m_BindingPoint; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
		uint32_t m_BindingPoint;
	};

}