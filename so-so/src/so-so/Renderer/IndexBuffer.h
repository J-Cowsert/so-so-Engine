#pragma once

#include <memory>

namespace soso {

	class IndexBuffer {
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetSize() const = 0;
		virtual uint32_t GetCount() const = 0;

		static std::shared_ptr<IndexBuffer> Create(void* indices, uint32_t size);
	};
}