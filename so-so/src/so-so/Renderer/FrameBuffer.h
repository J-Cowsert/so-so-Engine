#pragma once
#include "so-so/Core/core.h"
#include <vector>

namespace soso {

	enum class FrameBufferTextureFormat {
		None = 0,
		RGBA8,
		DEPTH24STENCIL8,
		RED_INTEGER
	};

	enum class TextureFiltering {
		Nearest,
		Linear
	};

	// TODO: Implement in backend
	enum class TextureWrap {
		Repeat,
		ClampToEdge,
		ClampToBorder
	};

	struct FrameBufferTextureConfig {

		FrameBufferTextureConfig() = default;

		FrameBufferTextureConfig(FrameBufferTextureFormat format,
			TextureFiltering filtering = TextureFiltering::Linear,
			TextureWrap wrap = TextureWrap::ClampToEdge)
			: TextureFormat(format), Filtering(filtering), Wrap(wrap) {
		}

		FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
		TextureFiltering Filtering = TextureFiltering::Linear;
		TextureWrap Wrap = TextureWrap::ClampToEdge;
	};

	struct FrameBufferAttachmentConfig {
		FrameBufferAttachmentConfig() = default;
		FrameBufferAttachmentConfig(std::initializer_list<FrameBufferTextureConfig> attachments)
			: Attachments(attachments) {
		}

		std::vector<FrameBufferTextureConfig> Attachments;
	};


	struct FrameBufferConfig {
		uint32_t Width = 0, Height = 0;
		FrameBufferAttachmentConfig Attachments;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
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

		virtual const FrameBufferConfig& GetConfig() const = 0;

		static std::shared_ptr<FrameBuffer> Create(const FrameBufferConfig& config);
	};
}