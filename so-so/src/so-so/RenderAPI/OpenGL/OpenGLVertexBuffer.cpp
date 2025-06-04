#include "sspch.h"
#include "OpenGLVertexBuffer.h"

#include "glad/glad.h"

namespace soso {

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size) 
		: m_Size(size)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void* indicies, uint32_t size) 
		: m_Size(size)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, indicies, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size) {

		SS_CORE_ASSERT(size <= m_Size, "Overflow");
		SS_CORE_ASSERT(data, "Data is null");
		glNamedBufferSubData(m_RendererID, 0, size, data);
	}
}
