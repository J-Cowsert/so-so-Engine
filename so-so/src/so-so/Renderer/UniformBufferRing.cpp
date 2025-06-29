#include "sspch.h"

#include "UniformBufferRing.h"
#include "Renderer.h"

#include "so-so/Core/Profiler.h"

namespace soso {

	std::shared_ptr<UniformBufferRing> UniformBufferRing::Create(uint32_t size, uint32_t bindingPoint, uint32_t framesInFlight) { 
		
		return std::make_shared<UniformBufferRing>(size, bindingPoint, framesInFlight); 
	}

	UniformBufferRing::UniformBufferRing(uint32_t size, uint32_t bindingPoint, uint32_t framesInFlight)
		: m_FramesInFlight(framesInFlight)
	{

		SS_PROFILE_FUNCTION();

		if (m_FramesInFlight == 0)
			m_FramesInFlight = Renderer::GetConfig().MaxFramesInFlight;

		m_RingBuffer.resize(m_FramesInFlight);
		for (size_t i = 0; i < m_FramesInFlight; i++) {

			m_RingBuffer[i] = UniformBuffer::Create(size, bindingPoint);
		}
	}

	std::shared_ptr<UniformBuffer> UniformBufferRing::GetCurrent() {

		SS_PROFILE_FUNCTION();

		uint32_t frame = Renderer::GetCurrentFrameIndex();
		frame = std::clamp(frame, 0u, m_FramesInFlight - 1);
		return m_RingBuffer[frame];
	}

	std::shared_ptr<UniformBuffer> UniformBufferRing::Get(uint32_t frame) {

		SS_CORE_ASSERT(frame < m_RingBuffer.size(), "Out of bounds");
		return m_RingBuffer[frame];
	}
}