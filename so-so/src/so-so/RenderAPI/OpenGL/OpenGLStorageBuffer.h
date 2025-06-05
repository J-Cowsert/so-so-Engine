#pragma once

#include "so-so/Renderer/StorageBuffer.h"

namespace soso {


	class OpenGLStorageBuffer : public StorageBuffer {
	public:
		OpenGLStorageBuffer(uint32_t size, uint32_t bindingPoint);
		virtual ~OpenGLStorageBuffer();

		void Bind() override;

		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

		uint32_t GetBindingPoint() const override { return m_BindingPoint; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
		uint32_t m_BindingPoint;
	};

}