#pragma once

#include "so-so/Renderer/FrameBuffer.h"

namespace soso {

	class OpenGLFrameBuffer : public FrameBuffer {
	public:
		OpenGLFrameBuffer(const FrameBufferConfig& config);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		void ClearAttachment(uint32_t attachmentIndex, int value) override;

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { 
			SS_CORE_ASSERT(index < m_ColorAttachments.size(), "index < m_ColorAttachments.size()"); return m_ColorAttachments[index]; }

		uint32_t GetDepthAttachmentRendererID() const override { return m_DepthAttachment; }

		const FrameBufferConfig& GetConfig() const override { return m_Config; }

	private:
		uint32_t m_RendererID = 0;
		FrameBufferConfig m_Config;

		std::vector<FrameBufferTextureConfig> m_ColorAttachmentConfigs;
		FrameBufferTextureConfig m_DepthAttachmentConfig = FrameBufferTextureFormat::None;

		uint32_t m_DepthAttachment = 0;
		std::vector<uint32_t> m_ColorAttachments;
	};

}