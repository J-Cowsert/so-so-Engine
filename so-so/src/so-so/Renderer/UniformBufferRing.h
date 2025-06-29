#pragma once

#include "so-so/Core/Core.h"
#include "UniformBuffer.h"

namespace soso {

	class UniformBufferRing {
	public:
		static std::shared_ptr<UniformBufferRing> Create(uint32_t size, uint32_t bindingPoint, uint32_t framesInFlight = 0);

		UniformBufferRing(uint32_t size, uint32_t bindingPoint, uint32_t framesInFlight);
		~UniformBufferRing() = default;

		std::shared_ptr<UniformBuffer> GetCurrent();
		std::shared_ptr<UniformBuffer> Get(uint32_t frame);
		
	private:
		std::vector<std::shared_ptr<UniformBuffer>> m_RingBuffer;
		uint32_t m_FramesInFlight = 0;
	};
}