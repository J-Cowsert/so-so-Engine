#include "sspch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace soso {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t bindingPoint) 
		: m_Size(size), m_BindingPoint(bindingPoint)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer() {

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::Bind() {

		glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {

		SS_CORE_ASSERT(offset + size <= m_Size, "overflow");

		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

}