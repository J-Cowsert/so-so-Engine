#pragma once
#include "so-so/Core/Core.h"

namespace soso {

	class UniformBuffer {
	public:
		static std::shared_ptr<UniformBuffer> Create(uint32_t size, uint32_t bindingPoint);
		virtual ~UniformBuffer() = default;

		virtual void Bind() = 0;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		virtual uint32_t GetBindingPoint() const = 0;
	};

}