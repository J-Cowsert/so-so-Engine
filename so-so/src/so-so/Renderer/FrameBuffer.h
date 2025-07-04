#pragma once

#include "so-so/Core/Core.h"

#include "Texture.h" // SamplerWrap, SamplerFilter, ImageFormat

#include <vector>

namespace soso {

	// TODO: Implement SamplerFilter and SamplerWrap in backend
	struct FrameBufferTextureConfig {

		FrameBufferTextureConfig() = default;

		FrameBufferTextureConfig(ImageFormat format, SamplerFilter filtering = SamplerFilter::Linear, SamplerWrap wrap = SamplerWrap::Clamp)
			: ImageFormat(format), Filtering(filtering), Wrap(wrap) 
		{
		}

		ImageFormat ImageFormat;

		SamplerFilter Filtering;
		SamplerWrap Wrap;
	};

	struct FrameBufferAttachmentConfig {

		FrameBufferAttachmentConfig() = default;

		FrameBufferAttachmentConfig(std::initializer_list<FrameBufferTextureConfig> attachments)
			: Attachments(attachments) 
		{
		}

		std::vector<FrameBufferTextureConfig> Attachments;
	};

	struct FrameBufferConfig {

		uint32_t Width = 0, Height = 0;
		FrameBufferAttachmentConfig Attachments;
		uint32_t Samples = 1;
	};

	class FrameBuffer {
	public:
		virtual ~FrameBuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
		virtual uint32_t GetDepthAttachmentRendererID() const = 0;

		virtual const FrameBufferConfig& GetConfig() const = 0;

		static std::shared_ptr<FrameBuffer> Create(const FrameBufferConfig& config);
	};
}