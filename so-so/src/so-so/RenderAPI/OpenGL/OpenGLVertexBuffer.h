#pragma once

#include "so-so/Renderer/VertexBuffer.h"

namespace soso {

	class OpenGLVertexBuffer : public VertexBuffer {
	public:

		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(void* indicies, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		OpenGLVertexBuffer(const OpenGLVertexBuffer& other) = delete;
		OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer&) = delete;

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* data, uint32_t size) override;

		const VertexBufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_RendererID;
		VertexBufferLayout m_Layout;
		uint32_t m_Size;
	};
}