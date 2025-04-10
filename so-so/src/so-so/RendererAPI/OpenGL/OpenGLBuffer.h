#pragma once

#include "so-so/Renderer/Buffer.h"

namespace soso {

	class OpenGLVertexBuffer : public VertexBuffer {
	public:

		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(void* indicies, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		void Bind() const override;
		void Unbind() const override;

		const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;

		uint32_t m_Size;
	};

	class OpenGLIndexBuffer : public IndexBuffer {
	public:

		OpenGLIndexBuffer(void* indicies, uint32_t size);
		virtual ~OpenGLIndexBuffer();

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetSize() const override { return m_Size; }
		uint32_t GetCount() const override { return m_Size / sizeof(uint32_t); }

	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
	};
}