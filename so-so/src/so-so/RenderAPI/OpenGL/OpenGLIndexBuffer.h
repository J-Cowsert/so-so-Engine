#pragma once
#include "so-so/Renderer/IndexBuffer.h"

namespace soso {

	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer(uint32_t size);
		OpenGLIndexBuffer(void* indices, uint32_t size);
		virtual ~OpenGLIndexBuffer();

		OpenGLIndexBuffer(const OpenGLIndexBuffer& other) = delete;
		OpenGLIndexBuffer& operator=(const OpenGLIndexBuffer&) = delete;

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* data, uint32_t size) override;

		uint32_t GetSize() const override { return m_Size; }
		uint32_t GetCount() const override { return m_Size / sizeof(uint32_t); }

	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
	};

}	