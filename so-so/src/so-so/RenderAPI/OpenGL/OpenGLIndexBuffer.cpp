#include "sspch.h"
#include "OpenGLIndexBuffer.h"

#include "glad/glad.h"

namespace soso {

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size)
		:m_Size(size)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(void* indices, uint32_t size)
		:m_Size(size) 
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer() {
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(const void* data, uint32_t size) {

		SS_CORE_ASSERT(size <= m_Size, "Overflow");
		SS_CORE_ASSERT(data, "Data is null");
		glNamedBufferSubData(m_RendererID, 0, size, data);
	}
}