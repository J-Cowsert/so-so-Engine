#include "sspch.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace soso {

	namespace Utils {

		static GLenum HazelImageFormatToGLDataFormat(ImageFormat format) {
			switch (format) {
				case ImageFormat::RGB8:  return GL_RGB;
				case ImageFormat::RGBA8: return GL_RGBA;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

		static GLenum HazelImageFormatToGLInternalFormat(ImageFormat format) {
			switch (format) {
				case ImageFormat::RGB8:  return GL_RGB8;
				case ImageFormat::RGBA8: return GL_RGBA8;
			}

			SS_CORE_ASSERT(false, "");
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureProvision& provision, Buffer data)
		: m_Provision(provision), m_Width(m_Provision.Width), m_Height(m_Provision.Height) {

		m_InternalFormat = Utils::HazelImageFormatToGLInternalFormat(m_Provision.Format);
		m_DataFormat = Utils::HazelImageFormatToGLDataFormat(m_Provision.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		if (data)
			SetData(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D() {

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(Buffer data) {

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		SS_CORE_ASSERT(data.Size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const {

		glBindTextureUnit(slot, m_RendererID);
	}
}