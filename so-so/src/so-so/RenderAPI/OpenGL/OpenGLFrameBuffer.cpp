#include "sspch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace soso {

	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils {

		static GLenum SosoImageFormatToGL(ImageFormat format) {
			
			switch (format) {
			case ImageFormat::R8:      return GL_R8;      // Single-channel
			case ImageFormat::RGB8:    return GL_SRGB8;
			case ImageFormat::RGBA8:   return GL_RGBA8;   // Linear RGBA
			case ImageFormat::RGBA16F: return GL_RGBA16F; // Floating-point RGBA
			case ImageFormat::RGBA32F: return GL_RGBA32F;

			case ImageFormat::SRGB:    return GL_SRGB8;
			case ImageFormat::SRGBA:   return GL_SRGB8_ALPHA8;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static bool IsDepthFormat(ImageFormat format) {
			
			switch (format) {

				case ImageFormat::DEPTH24STENCIL8:
				case ImageFormat::DEPTH32F: return true;
			}

			return false;
		}

		static GLenum TextureTarget(bool multisampled) {
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count) {
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id) {
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index) {

			bool multisampled = samples > 1;
			if (multisampled) {

				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else {

				glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {

			bool multisampled = samples > 1;

			if (multisampled) {

				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else {

				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}
	}

	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferConfig& config) 
		: m_Config(config)
	{
		for (auto& textureConfig : m_Config.Attachments.Attachments) {

			if (!Utils::IsDepthFormat(textureConfig.Format))
				m_ColorAttachmentConfigs.emplace_back(textureConfig);
			else
				m_DepthAttachmentConfig = textureConfig;
		}

		Invalidate();
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer() {

		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFrameBuffer::Invalidate() {

		if (m_RendererID) {
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		bool multisample = m_Config.Samples > 1;

		if (m_ColorAttachmentConfigs.size()) {

			m_ColorAttachments.resize(m_ColorAttachmentConfigs.size());
			Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++) {

				Utils::BindTexture(multisample, m_ColorAttachments[i]);

				switch (m_ColorAttachmentConfigs[i].Format) {

					case ImageFormat::RGBA8:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Config.Samples, GL_RGBA8, GL_RGBA, m_Config.Width, m_Config.Height, i);
						break;

					case ImageFormat::RGBA32F: 
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Config.Samples, GL_RGBA32F, GL_RGBA, m_Config.Width, m_Config.Height, i);
						break;

					case ImageFormat::RED_INTEGER:
						Utils::AttachColorTexture(m_ColorAttachments[i], m_Config.Samples, GL_R32I, GL_RED_INTEGER, m_Config.Width, m_Config.Height, i);
						break;
				}
			}
		}

		if (m_DepthAttachmentConfig.Format != ImageFormat::None) {

			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);

			switch (m_DepthAttachmentConfig.Format) {

				case ImageFormat::DEPTH24STENCIL8:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Config.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Config.Width, m_Config.Height);
					break;

				case ImageFormat::DEPTH32F:
					Utils::AttachDepthTexture(m_DepthAttachment, m_Config.Samples, GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, m_Config.Width, m_Config.Height);
					break;
			}
		}

		if (m_ColorAttachments.size() > 1) {

			SS_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Too Many Attachments");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty()) {

			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		SS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Bind() {

		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Config.Width, m_Config.Height);
	}

	void OpenGLFrameBuffer::Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height) {

		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize) {
			SS_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_Config.Width = width;
		m_Config.Height = height;

		Invalidate();
	}

	int OpenGLFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) {

		SS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value) {

		SS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "");

		auto& config = m_ColorAttachmentConfigs[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, Utils::SosoImageFormatToGL(config.Format), GL_INT, &value);
	}
}
