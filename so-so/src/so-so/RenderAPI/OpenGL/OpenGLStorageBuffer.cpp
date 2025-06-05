#include "sspch.h"

#include "OpenGLStorageBuffer.h"

#include "glad/glad.h"

namespace soso {

	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t size, uint32_t bindingPoint)
		: m_Size(size), m_BindingPoint(bindingPoint)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_RendererID);
	}

	OpenGLStorageBuffer::~OpenGLStorageBuffer() {

		glDeleteBuffers(1, &m_RendererID);
	}
	
	void OpenGLStorageBuffer::Bind() {
	
		glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_RendererID);
	}

	void OpenGLStorageBuffer::SetData(const void* data, uint32_t size, uint32_t offset)	{

		SS_CORE_ASSERT(offset + size <= m_Size, "overflow");

		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

}