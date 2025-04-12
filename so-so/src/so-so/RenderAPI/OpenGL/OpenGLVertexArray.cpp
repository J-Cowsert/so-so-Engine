#include "sspch.h"
#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace soso {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case soso::ShaderDataType::Float:    return GL_FLOAT;
			case soso::ShaderDataType::Float2:   return GL_FLOAT;
			case soso::ShaderDataType::Float3:   return GL_FLOAT;
			case soso::ShaderDataType::Float4:   return GL_FLOAT;
			case soso::ShaderDataType::Mat3:     return GL_FLOAT;
			case soso::ShaderDataType::Mat4:     return GL_FLOAT;
			case soso::ShaderDataType::Int:      return GL_INT;
			case soso::ShaderDataType::Int2:     return GL_INT;
			case soso::ShaderDataType::Int3:     return GL_INT;
			case soso::ShaderDataType::Int4:     return GL_INT;
			case soso::ShaderDataType::Bool:     return GL_BOOL;
		}

		SS_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const {
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const {
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {

		SS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		for (const auto& element : vertexBuffer->GetLayout()) {

			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentSize(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				vertexBuffer->GetLayout().GetStride(),
				(const void*)element.Offset);

			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}
}